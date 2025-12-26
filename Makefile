all: PCApp WiiApp

PCApp: 
	cd DATEditor && $(MAKE)	

WiiApp: 
	cd WiiRremote && $(MAKE)

.PHONY: clean

clean:
	cd DATEditor && $(MAKE)	clean
	cd WiiRremote && $(MAKE) clean
