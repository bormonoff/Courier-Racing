## Introduction
<table>
<tr>
<td>
Courier racing - is a browser multiplayer online game. At the moment the game is under development.

First public beta-test was sucessfully carried out on 7th April 2023. You can easily test this server on your mashine 
via the followed instruction.
 </td>
 </tr>
 </table>
 
## How to play
<table>
<tr>
<td>

 - Connect to `http://127.0.0.1:8080` if you launch server locally, instead change IP to your VM IP. 
 - `Choose a map` where you want to play
 - `Run and collect items` with other people. At the and `wait`. Server automaticaly ends your game and makes info about winners.
 </td>
 </tr>
 </table>
 
 
## Installation guide
<table>
<tr>
<td>
 
 - `Install docker and Docker-compose` on your mashine. Installation depends on your linux distribution. All my mashines use arch-linux and I simply
 download docker using pacman pocket manager. 
 - `Clone this repository` on your computer and `change terminal direcory to this repo root`.
 - `Write the following commands:` 
 ```
 - sudo chmod 777 docker/save 
 - sudo docker-compose build
 - sudo docker-compose up 
 ```
 </td>
 </tr>
 </table>

### Instruments used in this game 
`Frontend is written using JavaScript. Backend using C++.` In the project the following libraries were used:

 - `BOOST`
   - Asio
   - Beast
   - Log
   - Program options
   - Serialize
   - JSON 
   - UUID
 - `Catch2` 
 - `libpqxx` 

## License

Courier-Recing is <a href="./LICENSE">Apache License.</a>

---

*If you have any questions, please feel free to contact me on my personal telegram: `@bomron_off`*