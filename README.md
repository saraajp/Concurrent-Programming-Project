# Concurrent-Programming-Project
This is a project I did for my Concurrent Programming class

This project applies filters to any .jpeg images in a directory specified by the user.

The user may also write in the initial arguments the nº of threads it wants the code to run. The threads are synchronized in a way to avoid running conditios.




---------------------------------------------------------------------------------
How to run the code:
./old-photo-parallel-B ./<directory_of_the_images> <nº of threads> <-name|-size>

(Ex: ./old-photo-parallel-B ./myimages 4 -name)
(it processes the images in ./myimages folder, using 4 threads, in alphabetic order)
---------------------------------------------------------------------------------
