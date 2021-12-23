Run:    ./fs-fuse -t 2097152 -a virtual-disk -f '-d -s mount-point'

Error:  `cannot access mount-point: Transport endpoint is not connected`
Fix:    Use command `fusermount -uz mount-point` to force unmount, and then u can delete or
        do whatever with that folder.