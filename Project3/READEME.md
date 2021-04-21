# OperatingSystems

## Use C++ compiler and run on Linux (Tested on Ubuntu)
Run Server First Before Client
### Server:
`g++ -o srv Server.cpp -lpthread` <br>
To Run <br>
`./srv` <br>

### Client:
`g++ -o client Client.cpp` <br>
To Run You Must Use Command Line Arguments For The Message As Follows <br>
`./client """Message With Spaces"""` <br>
