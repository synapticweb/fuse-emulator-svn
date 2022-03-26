A spectrum emulator with support for HC2000 romanian spectrum clone. 
I added command line support for the second drive (B:) of the if1 interface of hc2000 machine.

The machine must be run like this:

`fuse -m hc2000 --interface1-fdc --if1disk-a disk-a-image --if1disk-b disk-b-image --drive-if1-fdc-b-type='Double-sided 80 track'`
