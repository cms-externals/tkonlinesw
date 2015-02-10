#!/bin/bash
echo "This shell window is to open an ssh tunnel to the cvs repository.
You should enter the password 'fed9u' and then hit return.
When you are done leave this window open and go back to the original
window and hit enter to continue the installation..."
ssh -L 24001:localhost:2401 anonymous@heppc41.hep.ph.ic.ac.uk

