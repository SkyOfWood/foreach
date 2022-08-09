LIBS = -lpthread
OBJS = foreach.cpp
ARGS = -Wall -g
foreach: ${OBJS}
	g++ -o foreach ${OBJS} ${LIBS} ${ARGS}
clean:
	rm -rf foreach ${OBJS}
install:
	install -m 755 foreach ${RPM_INSTALL_ROOT}/usr/local/bin/foreach
	
