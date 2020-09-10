# NetworkingCoursework:
Networking coursework during my course at abertay where I created a game where you can fire at a target and use a shield to bounce back shots.

Video Link:
https://youtu.be/0DpAjEppBFA

## Description:
The game is a simple game where a server needs to be run first and then multiple clients can connect to it. 
Each client has a Player associated with it that can fire Bullets that are associated with the player. 

Communication is done with TCP from client to server and a stream UDP from server to client. 
This is to prevent cheating and also ensure that all clients are receiving the same instance of the game.
I use dead reckoning and interpolation for rendering on the client. The server world is the one that decides the game rules and positioning of all objects in the world.

## Instructions:
Start by running the application and inputting ‘s’ in the terminal to start a Server. 
Then open another instance of the application and put in ‘c’ and input a username and server ip address to start a Client.
- If a Bullet hits a player, it reduces it’s health by 1 if it is not shielded. 
- If it is shielded, the bullet will change colour to the player’s colour and bounce off of it.


## Controls:
- W or Right Click: Move the player towards current mouse position
- S or Middle Mouse: Put a shield and prevent player movement
- SPACE or Left Click: Fire a bullet towards the mouse position

## Screenshots
![Server and log](https://github.com/RohanMenon92/NetworkingCoursework/blob/master/Screenshots/ServerLog.PNG)

![Clients and log](https://github.com/RohanMenon92/NetworkingCoursework/blob/master/Screenshots/Clients.PNG)
