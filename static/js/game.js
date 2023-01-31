const loadedTiles = {};

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

function makeActor(scene, x, y) {
  const fbxLoader = new THREE.FBXLoader();
  fbxLoader.load('assets/pug.fbx', (fbx) => {
      /*var bb = new THREE.Box3();
      bb.setFromObject( fbx );

      var w  = (bb.max.x - bb.min.x) * scale;
      var t  = (bb.max.y - bb.min.y) * scale;
      var h  = (bb.max.z - bb.min.z) * scale;*/
      const scale = 0.002;

      fbx.scale.set(scale,scale,scale);
      fbx.position.set(x + .5, roadH, y + .5);
      scene.add(fbx);
  });   
}

function gameserverMain() {
  const basementH = 0.5;

  const renderer = new THREE.WebGLRenderer();
  renderer.depthTest = false;

  renderer.shadowMap.enabled = true;
  renderer.setSize( window.innerWidth, window.innerHeight );
  const canvas  = renderer.domElement;
  document.body.appendChild(  canvas );

  const fov = 45;
  const aspect = 2;  // the canvas default
  const near = 0.1;
  const far = 100;
  const camera = new THREE.PerspectiveCamera(fov, aspect, near, far);
  camera.position.set(0, 10, 20);

  const controls = new THREE.OrbitControls(camera, canvas);
  controls.target.set(0, 5, 0);
  controls.update();

  const scene = new THREE.Scene();
  scene.background = new THREE.Color('#DEFEFF');

  {
    const w = xmax - xmin + 1;
    const h = ymax - ymin + 1;
    const basementGeo = new THREE.BoxBufferGeometry(w, basementH, h);
    const basementMat = new THREE.MeshPhongMaterial({color: '#8AC'});
    const mesh = new THREE.Mesh(basementGeo, basementMat);
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

  makeActor(scene, 0, 0);

  function render(time) {
    const stime = time * 0.001;

    if (resizeRendererToDisplaySize(renderer)) {
      const canvas = renderer.domElement;
      camera.aspect = canvas.clientWidth / canvas.clientHeight;
      camera.updateProjectionMatrix();
    }

    renderer.render(scene, camera);

    requestAnimationFrame(render);
  }

  requestAnimationFrame(render);
}