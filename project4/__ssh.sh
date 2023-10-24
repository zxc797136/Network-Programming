#! /usr/bin/bash

#scp -r $(pwd) wxjiang@nplinux11.cs.nctu.edu.tw:/u/gcs/111/311552048/311552048
scp -r /home/wx/311552048_np_project4 wxjiang@nplinux2.cs.nctu.edu.tw:/u/gcs/111/311552048

echo " ----- Login Remote Access -----"
ssh -t wxjiang@nplinux2.cs.nctu.edu.tw "cd 311552048_np_project4; make; /bin/bash"