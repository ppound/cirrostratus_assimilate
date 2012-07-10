Allows Islandora to communicate with Fedora Cloudsync and Duracloud

Requires the php5 mcrypt library

To connect with cloudsync we must store a url, username and password.  We
cannot use a oneway hash of the password as we have to send the password
to cloudsync, so we encrypt it using mcrypt.  

The includes directory of this module includes a file called key.txt.  By default islandora_vault
tries to read the first line of this file as a file path.  If the file exists it uses
the file as a key.  If the file does not exist it uses the text of the key.txt file as 
the key.  

If you supply a file it has to be readable by php but should be outside of the webservers
 web accesible directories. 


If the key changes after you have configured islandora_vault you will have to resave your password 
so islandora_vault can send the correct password to cloudsync. 