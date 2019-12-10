Multiplayer game example
Start by running the application and inputting ‘s’ in the terminal to start a Server. 
Then open another instance of the application and put in ‘c’ and input a username and server ip address to start a Client.

The game is a simple game where a server needs to be run first and then multiple clients can connect to it. 
Each client has a Player class associated with it that can fire Bullets that are associated with the player. 
If a Bullet hits a player, it reduces it’s health by 1 if it is not shielded. 
If it is shielded, the bullet will change colour to the player’s colour and bounce off of it.

The controls for client are:
W or Right Click: Move the player towards current mouse position
S or Middle Mouse: Put a shield and prevent player movement
SPACE or Left Click: Fire a bullet towards the mouse position