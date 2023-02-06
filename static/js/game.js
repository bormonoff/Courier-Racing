const loadedTiles = {};
let loadedActor = undefined;
let globalScene;
let mixers = [];
let playerId;
let camera;

function loadRoadTiles() {
  const loader = new THREE.TextureLoader();
  loader.setPath( 'assets/' );
  for(k of ['nt', 't', 'tr', 'v', 'vh']) {
     loadedTiles[k] = new THREE.MeshBasicMaterial({map: loader.load('road_' + k + '.png')});
  }
}

loadRoadTiles();
const roadEdge = new THREE.MeshPhongMaterial({color: '#AAA'});
const roadH = 0.55;

function convDirection(d) {
  const dict = {
    R:Math.PI/2,
    U:Math.PI,
    L:Math.PI*3/2,
    D:0
  };
  return dict[d];
}

class KeyState {
  constructor() {
    this.pressedKeys = [];
  }

  keyDown(key) {
    var index = this.pressedKeys.indexOf(key);
    if (index !== -1)
      return false;

    this.pressedKeys.push(key);
    return true;
  }
  keyUp(key) {
    var index = this.pressedKeys.indexOf(key);
    if (index === -1)
      return false;

    this.pressedKeys.splice(index, 1);
    return true;
  }

  getKeyMask() {
    return this.pressedKeys.join('');
  }

  getLastKey() {
    if (this.pressedKeys.length == 0)
      return "";
    return this.pressedKeys[this.pressedKeys.length - 1];
  }
}

class GameState {
  constructor(scene) {
    let self = this;
    this.players={};
    this.scene = scene;
    this.started = false;
    this.stateLoaded = false;
    this.playersLoaded = false;
    this.updateInProgress = false;
    this.playersSuncInProgress = false;
    this.ticks = 0;
    this.posUpdateInterval = 1;
    this.playersUpdateInterval = 50;
    this.keyState = new KeyState();
    this.currentState = {};

    this._updateState(function() {
      self.stateLoaded = true;
      self._startGame();
    });
    this._syncPlayers(function() {
      self.playersLoaded = true;
      self._startGame();
    });
  }

  tick() {
    this.ticks++;

    let self = this;

    if (!this.started)
      return false;

    if (this.ticks % this.posUpdateInterval == 0 && !this.updateInProgress) {
      this._updateState(function() {
        self._applyDesiredState();
        self._instantApplyState();
      });
    }

    if (this.ticks % this.playersUpdateInterval == 0 && !this.playersSuncInProgress) {
      this._syncPlayers(function(){});
    }

    self._interpolateState();
    self._instantApplyState();
  }

  keyUp(key) {
    if (this.keyState.keyUp(key)) {
      this._pressKey(this.keyState.getLastKey(), function(){})
    }
  }

  keyDown(key) {
    if (this.keyState.keyDown(key)) {
      this._pressKey(this.keyState.getLastKey(), function(){})
    }
  }

  _pressKey(keys, then) {
    $.post({
      url: '/api/v1/game/player/action',
      dataType: 'json',
      contentType:"application/json",
      data: JSON.stringify({
        move: keys
      }),
      beforeSend: function (xhr) {
        xhr.setRequestHeader ("Authorization", "Bearer " + Cookies.get('authToken'));
      }
    }).done(function(x){
      then();
    })
  }

  _startGame() {
    if (this.started || !this.stateLoaded || !this.playersLoaded) {
      return;
    }
    this.started = true;
    this._applyDesiredState();
    this._instantApplyState();

    if (this.desiredState.players[playerId] !== undefined) {
      const thisPlayer = this.desiredState.players[playerId];
      moveCameraTo(thisPlayer['pos'][0], thisPlayer['pos'][1]);
    }
  }

  _interpolateState() {
    const delta = performance.now() - this.stateTime;
    const speedMul = 1/10000;
    Object.entries(this.desiredState['players']).forEach(([id, playerPos]) => {
      this.currentState['players'][id]['pos'][0] += delta * this.currentState['players'][id]['speed'][0] * speedMul;
      this.currentState['players'][id]['pos'][1] += delta * this.currentState['players'][id]['speed'][1] * speedMul;
    });
  }

  _syncPlayers(then) {
    this.playersSuncInProgress = true;
    let self = this;
    $.get({
      url: '/api/v1/game/players',
      dataType: 'json',
      beforeSend: function (xhr) {
        xhr.setRequestHeader ("Authorization", "Bearer " + Cookies.get('authToken'));
      }
    }).done(function(x){
      self._updatePlayersList(x);
      then();
    })

    this.playersSuncInProgress = false;
  }

  _updatePlayersList(ps) {
    this.updateInProgress = true;

    let self = this;
    let allPlayers=[];

    Object.entries(ps).forEach(([id, playerData]) => {
      if (self.players[id]===undefined) {
        self._addPlayer(id, playerData);
      }
      allPlayers.push(id);
    });

    let playersToDelete=[];
    for(const id in this.players) {
      if (allPlayers.indexOf(id) < 0) {
        playersToDelete.push(id);
      }
    }

    for(const id of playersToDelete) {
      self._deletePlayer(id);
    }

    this.updateInProgress = false;
  }

  _addPlayer(id, playerData) {
    let self = this;
    self.players[id] = {
      object: addActor(this.scene),
      data: playerData
    };
  }

  _deletePlayer(id) {
    let self = this;
    delete self.players[id];
  }

  _movePlayerTo(id, playerPos) {
    let self = this;
    moveActor(self.players[id].object, playerPos['pos'][0], playerPos['pos'][1], playerPos['converted_dir']);
    setAnimation(self.players[id].object, playerPos['speed'][0] != 0 || playerPos['speed'][1] != 0)

    if(id == playerId) {
      moveCameraTo(playerPos['pos'][0], playerPos['pos'][1]);
    }
  }

  _instantApplyState() {
    let self = this;
    Object.entries(this.currentState['players']).forEach(([id, playerPos]) => {
      if (self.players[id]===undefined) {
        return;
      }
      self._movePlayerTo(id, playerPos);
    });
  }

  _updateState(then) {
    let self = this;
    $.get({
      url: '/api/v1/game/state',
      dataType: 'json',
      beforeSend: function (xhr) {
        xhr.setRequestHeader ("Authorization", "Bearer " + Cookies.get('authToken'));
      }
    }).done(function(x){
      self.desiredState = x;
      self.stateTime = performance.now();
      then();
    })
  }

  _applyDesiredState() {
    const old_players = this.currentState['players'] !== undefined ? this.currentState['players'] : {};
    const new_players = {};

    const pi = Math.PI;
    const rot_speed = pi / 300;

    Object.entries(this.desiredState['players']).forEach(([id, playerPos]) => {
      new_players[id] = playerPos;

      const newDir = convDirection(playerPos['dir']);

      if (old_players[id] !== undefined) {
        if (old_players[id]['desired_dir'] != newDir) {
          new_players[id]['converted_dir'] = old_players[id]['converted_dir'];
          new_players[id]['base_dir'] = old_players[id]['converted_dir'];
          new_players[id]['desired_dir'] = newDir;
          new_players[id]['dir_time'] = performance.now();
        }
        else {
          new_players[id]['base_dir'] = old_players[id]['base_dir'];
          new_players[id]['desired_dir'] = old_players[id]['desired_dir'];
          new_players[id]['dir_time'] = old_players[id]['dir_time'];

          const delta = performance.now() - new_players[id]['dir_time'];
          const rot_delta = new_players[id]['desired_dir'] - new_players[id]['base_dir'];
          var rot_dir;
          if (rot_delta <= -pi || (0 <= rot_delta && rot_delta < pi)) {
            rot_dir = 1;
          }
          else {
            rot_dir = -1;
          }

          const abs_delta = Math.abs(rot_delta);
          const real_rot_delta = abs_delta >= pi ? (2*pi - abs_delta) : abs_delta;

          if (delta * rot_speed > real_rot_delta) {
            new_players[id]['converted_dir'] = old_players[id]['desired_dir'];
          }
          else {
            new_players[id]['converted_dir'] = old_players[id]['base_dir'] + rot_dir * 
              delta * rot_speed;
          }
        }
      }
      else {
        new_players[id]['converted_dir'] = newDir;
        new_players[id]['desired_dir'] = newDir;
        new_players[id]['base_dir'] = newDir;
        new_players[id]['dir_time'] = performance.now();
      }
    });

    this.currentState['players'] = new_players;
  }
}

function makeRoadTile(tile) {
  const edge_count = (tile['u'] ? 1 : 0) + (tile['r'] ? 1 : 0) + (tile['d'] ? 1 : 0) + (tile['l'] ? 1 : 0);
  let top;
  let rot;
  switch(edge_count) {
    case 0: top = roadEdge; rot = 0; break;
    case 1: 
      pos = (tile['r'] ? 1 : 0) + (tile['d'] ? 2 : 0) + (tile['l'] ? 3 : 0);
      top = loadedTiles['t']; 
      rot = pos * 90; 
      break;
    case 2:
      if (tile['r'] && tile['l']) {
        top = loadedTiles['v']; 
        rot = 90; 
      }
      else if (tile['u'] && tile['d']) {
        top = loadedTiles['v']; 
        rot = 0; 
      }
      else {
        pos = (tile['r'] && tile['d'] ? 3 : 0) + (tile['d'] && tile['l'] ? 2 : 0) + (tile['l'] && tile['u'] ? 1 : 0);
        top = loadedTiles['tr']; 
        rot = pos * 90; 
      }
      break;
    case 3:
      pos = (!tile['r'] ? 1 : 0) + (!tile['d'] ? 2 : 0) + (!tile['l'] ? 3 : 0);
      top = loadedTiles['nt']; 
      rot = pos * 90; 
      break;
    case 4:
      top = loadedTiles['vh']; 
      rot = 0;
      break;
  }
  const materials = [ roadEdge, roadEdge, top, roadEdge, roadEdge, roadEdge ];

  const geo = new THREE.BoxBufferGeometry(1, roadH, 1);
  const mesh = new THREE.Mesh(geo, materials);
  mesh.rotation.y = rot / 360 * 2 * Math.PI;
  mesh.receiveShadow = true;

  return mesh;
}

function makeRoads(scene) {
  for(y = ymin; y<=ymax;++y) {
    for(x = xmin; x<=xmax;++x) {
      const tile = map_tiles[y-ymin][x-xmin];
      if (!tile['road']) continue;

      const mesh = makeRoadTile(tile); ;//new THREE.Mesh(geo, mat);
      mesh.position.set(x+.5, roadH/2., y+.5);
      scene.add(mesh);
    }
  }
}

function makeBuildings(scene) {
  const buildingH = 3;
  const materials = ['#f88', '#ff8', '#f8f', '#8f8', '#88f', '#888'].map(c => new THREE.MeshPhongMaterial({color: c}));

  for(const b of map['buildings']) {
    const x = b['x'];
    const y = b['y'];
    const w = b['w'];
    const h = b['h'];
    const geo = new THREE.BoxBufferGeometry(w, buildingH , h);
    const mesh = new THREE.Mesh(geo, materials);
    mesh.position.set(x+w/2., buildingH/2., y+h/2.);
    scene.add(mesh);
  }
}

function loadActor(/*, x, y*/then) {
  const fbxLoader = new THREE.FBXLoader();
  fbxLoader.load('assets/pug.fbx', (fbx) => {
      then(fbx);
  });   
}

function getRandomInt(max) {
  return Math.floor(Math.random() * max);
}

function getRandomColor(max) {
  r = getRandomInt(max);
  g = getRandomInt(max);
  b = getRandomInt(max);

  return (r*256*256 + g*256 + b);
}

function addActor(scene) {
  const scale = 0.002;

  actor = new Object();
  actor.object = THREE.SkeletonUtils.clone(loadedActor);
  actor.object.visible = false;
  actor.object.scale.set(scale, scale, scale);

  animate(actor);
  paint(actor, getRandomColor(127)); // 0x442233

  scene.add(actor.object);
  return actor;
}

function moveCameraTo(x, y) {
  camera.position.set(x + 6.7, 8.5, y + 6.7);
}

function moveActor(actor, x, y, dir) {
  actor.object.visible = true;
  actor.object.position.set(x + .5, roadH, y + .5);
  actor.object.rotation.y = dir;
}

function setAnimation(actor, moving) {
  if(actor.moving == moving)
    return;

  if (moving) actor.action.play();
  else actor.action.stop();

  actor.moving = moving;
}

function isReady() {
  if (loadedActor===undefined) return false;

  return true;
}

function animate(model) {
  const object = model.object;

  m = new THREE.AnimationMixer(object);
  mixers.push(m);
  model.action = m.clipAction( loadedActor.animations[ 0 ] );
  model.moving = false;
}

function paint(model, color) {
  var mat = new THREE.MeshPhongMaterial({
    color: color,
    //skinning: true ,
    //morphTargets :true,
    specular: 0x1d1c3a,
    reflectivity: 0.8,
    shininess: 20,} );

  model.object.traverse( function ( child ) {
    if ( child.isMesh ) {
      child.castShadow = true;
      //child.receiveShadow = true;
      child.material = mat;
    }
  });
}

function setupKeyEvents(gameState) {
  const arrowUp = 38;
  const arrowDown = 40;
  const arrowLeft = 37;
  const arrowRight = 39;
  const keyMap = {
    [arrowUp]:"U",
    [arrowRight]: "R",
    [arrowDown]: "D",
    [arrowLeft]: "L"
  };
  $(document).keydown( function(event) {
    if (keyMap[event.which] !== undefined) {
      event.preventDefault();
      gameState.keyDown(keyMap[event.which]);
    }
  }); 
  $(document).keyup( function(event) {
    if (keyMap[event.which] !== undefined) {
      event.preventDefault();
      gameState.keyUp(keyMap[event.which]);
    }
  }); 
}

function gameserverMain() {
  playerId = Cookies.get("playerId");

  const renderer = new THREE.WebGLRenderer( { antialias: true } );
  renderer.depthTest = false;

  renderer.shadowMap.enabled = true;
  renderer.setPixelRatio( window.devicePixelRatio );
  renderer.setSize( window.innerWidth, window.innerHeight );
  const canvas  = renderer.domElement;
  document.body.appendChild(  canvas );

  loadActor((object)=> {
    loadedActor = object;
  });

  const basementH = 0.5;
  const fov = 45;
  const aspect = 2;  // the canvas default
  const near = 0.1;
  const far = 2000;
  camera = new THREE.PerspectiveCamera(fov, aspect, near, far);
  camera.position.set(0, 10, 20);
  camera.rotation.set(-0.825, 0.574, 0.531);

  //const controls = new THREE.OrbitControls(camera, canvas);
  //controls.target.set(0, 5, 0);
  //controls.update();

  const scene = new THREE.Scene();
  scene.background = new THREE.Color('#DEFEFF');
  globalScene = scene;

  const clock = new THREE.Clock();

  {
    const w = xmax - xmin + 1;
    const h = ymax - ymin + 1;
    const basementGeo = new THREE.BoxBufferGeometry(w, basementH, h);
    const basementMat = new THREE.MeshPhongMaterial({color: '#8AC'});
    const mesh = new THREE.Mesh(basementGeo, basementMat);
    mesh.receiveShadow = true;
    mesh.position.set(w/2., basementH/2., h/2.);
    scene.add(mesh);
  }

  
  {
    const skyColor = 0xB1E1FF;  // light blue
    const groundColor = 0xB97A20;  // brownish orange
    const intensity = 1;
    const light = new THREE.HemisphereLight(skyColor, groundColor, intensity);
    scene.add(light);
  }

  makeRoads(scene);
  makeBuildings(scene);

  {
    const color = 0xFFFFFF;
    const intensity = 1;
    const light = new THREE.DirectionalLight(color, intensity);
    light.castShadow = true;
    light.position.set(-2.50, 8.00, -8.50);
    light.target.position.set(-5.50, 0.40, -45.0);

    light.shadow.bias = -0.004;
    light.shadow.mapSize.width = 2.048;
    light.shadow.mapSize.height = 2.048;

    scene.add(light);
    scene.add(light.target);
    const cam = light.shadow.camera;
    cam.near = 0.01;
    cam.far = 20.00;
    cam.left = -15.00;
    cam.right = 15.00;
    cam.top = 15.00;
    cam.bottom = -15.00;
  }

  function resizeRendererToDisplaySize(renderer) {
    const canvas = renderer.domElement;
    const width = canvas.clientWidth;
    const height = canvas.clientHeight;
    const needResize = canvas.width !== width || canvas.height !== height;
    if (needResize) {
      renderer.setSize(width, height, false);
    }
    return needResize;
  }

  let gameState = undefined;

  function render(time) {
    const stime = time * 0.001;
    const delta = clock.getDelta();

    if (gameState===undefined && isReady()) {
      gameState = new GameState(scene);
      setupKeyEvents(gameState);
    }

    if (gameState !== undefined) {
      gameState.tick();
    }

    if (resizeRendererToDisplaySize(renderer)) {
      const canvas = renderer.domElement;
      camera.aspect = canvas.clientWidth / canvas.clientHeight;
      camera.updateProjectionMatrix();
    }

    for (m of mixers) 
      m.update(delta);
    renderer.render(scene, camera);

    requestAnimationFrame(render);
  }

  requestAnimationFrame(render);
}