.SILENT:

all:
	echo "make build"
	echo "make install"
	echo "make clean"

clean:
	pio run --target clean
	rm -rfv .pio

build:
	pio run

install:
	pio run --target upload
