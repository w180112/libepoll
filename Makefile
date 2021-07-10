############################################################
# libepoll makefile
############################################################

######################################
# Set variable
######################################	
SUBDIR = lib/libutil src example
UTIL_INIT = util_init
EPOLL_INIT = epoll_init

BUILDSUBDIR = $(SUBDIR:%=build-%)
CLEANSUBDIR = $(SUBDIR:%=clean-%)

all: $(BUILDSUBDIR)

$(BUILDSUBDIR):
	${MAKE} -C $(@:build-%=%)

.PHONY: $(BUILDSUBDIR)

init: $(UTIL_INIT) $(EPOLL_INIT)

$(UTIL_INIT):
	cd lib/libutil && autoreconf --install && ./configure || exit 1

$(EPOLL_INIT):
	cd src && autoreconf --install && ./configure || exit 1

######################################
# Clean 
######################################
clean: $(CLEANSUBDIR)

$(CLEANSUBDIR):
	$(MAKE) -C  $(@:clean-%=%) clean