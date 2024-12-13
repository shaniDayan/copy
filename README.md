student@pc:~/Desktop/OS_WET1-main$ ./smash
smash> cat hi>>new
cat: hi: No such file or directory
smash> echo hi>> new
smash> cat new
hi
smash> chmod 0 new
smash> ls -l  new
---------- 1 student student 3 Dec 13 13:28 new
smash> ls > new
smash error: open failed: Permission denied
smash> rm new
rm: remove write-protected regular file 'new'? 
