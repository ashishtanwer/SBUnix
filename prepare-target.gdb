#set remotebreak 1
#set $connected=0
#shell sleep 5
#target remote localhost:1234
#set $connected=1
file ./kernel
#b start
# continue optional...

