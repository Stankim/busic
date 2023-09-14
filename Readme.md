#Busic - Compose Your Music in words.
This is a terminal application for both windows and linux for composing music in words rather than graphics. A graphical UI would be nice but for now the app accepts input in form of musical notes A-G. Down here is a list of all of them.
C	doh
C#
D	ray
D#
E	me
F	fah
F#
G	soh
G#
A	la
A#
B	te
Note that they are 12 notes matching 7 corresponding notes(doh ray me...).
The notes have 9 levels ie. 0 to 8. For example
C0, C1, G4, G#5 etc.
This makes all the available notes add up to 108, from C0 to B8.


#Execution

Run with -lm flag.
gcc main.c -o main -lm
