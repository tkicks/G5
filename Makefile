CLIBS= 		-lm -lglut -lGL -lGLU -lX11 -pthread # 2014
#CPATHS=	-L/usr/X11R6/lib -I/usr/X11R6/include # 2011
CPATHS=		-I/usr/include -L/usr/lib # 2014
CFLAGS=		$(CPATHS)$(CLIBS)

Kickham_Gillis_G5:		Kickham_Gillis_G5.cpp glm.c
		g++ glm.c Kickham_Gillis_G5.cpp -o Kickham_Gillis_G5 $(CFLAGS)

test:		test.cpp glm.c
		g++ glm.c test.cpp -o test $(CFLAGS)

clean:
		rm *.o
