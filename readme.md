---
title: Install PAM library

---

# PAM Password change Daemon 
## How It Works

- The Client Program prompts users to enter their username, old password, and new password.

- The client sends this information to the Daemon Server over a TCP socket.

- The Daemon Server authenticates the user using PAM.

- If authentication is successful, the daemon changes the user password by executing the chpasswd command with execve.

- The client receives a response indicating whether the password change was successful or not.


## Step 1. Install PAM library
```bash
sudo apt update
sudo apt install libpam0g-dev
```

## Step 2. Compile the code

**option 1.** Use Makefile to compile
```bash
make
```

**option 2.** Compile with gcc command
```bash
gcc daemon.c -o daemon -lpam -lpam_misc
gcc client.c -o client
```

## Step 3. Start the daemon server
```bash
sudo ./daemon
```
**NOTE** : Ensure you have root privileges; otherwise, the daemon cannot modify passwords.
## Step 4. Turn on client
```
./client
```
**NOTE** : Please use another terminal to run the client program so it can communicate with the daemon.


---
### Example Intput
```
Username: user1
Old Password: oldpass123
New Password: newpass123
```

### Expected Output
```
Password Changed!
```

If authentication fails, the output will be:
```
Authentication Failed!
```    
# passwd_daemon
