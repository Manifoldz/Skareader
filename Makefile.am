SUBDIRS = src

distclean-local:
	-./scripts/cleanall.sh

install-external:
	mkdir -p /usr/local/bin
	sudo cp $(top_srcdir)/install/bin/skareader /usr/local/bin
	chmod +x /usr/local/bin/skareader
	@echo "Binary file copied in your directory '/usr/local/bin'"