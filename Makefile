EXTENSION = h3

# extract extension version from .control file
EXTVERSION = $(shell grep default_version $(EXTENSION).control | \
	sed -e "s/default_version[[:space:]]*=[[:space:]]*'\([^']*\)'/\1/")

# h3 core library version to clone and statically link
LIBH3_VERSION = v3.5.0
# directory that h3 core repository is cloned into
LIBH3_DIR = libh3-$(LIBH3_VERSION)
# h3 static library location
LIBH3_FILE = $(LIBH3_DIR)/install/lib/h3.a
# sql files used for installation, update and testing
SQL_INSTALLS = $(wildcard sql/install/*.sql)
SQL_UPDATES = $(wildcard sql/updates/*.sql)
SQL_TESTS = $(wildcard test/sql/*.sql)
SQL_FULLINSTALL = sql/$(EXTENSION)--$(EXTVERSION).sql
SQL_UPDATETEST = sql/$(EXTENSION)--updatetest.sql

# a shared library to build from multiple source files (list object files in OBJS)
MODULE_big = $(EXTENSION)
# object files to be linked together
OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c))
# random files to install into $PREFIX/share/$MODULEDIR
DATA = $(SQL_UPDATES) $(SQL_FULLINSTALL) $(SQL_UPDATETEST)
# will be added to MODULE_big link line
SHLIB_LINK += -lh3 -L$(LIBH3_DIR)/install/lib
# will be added to CPPFLAGS
PG_CPPFLAGS += -I$(LIBH3_DIR)/install/include
# list of regression test cases (without suffix)
REGRESS = $(basename $(notdir $(SQL_TESTS)))
# additional switches to pass to pg_regress
REGRESS_OPTS = --inputdir=test --outputdir=test --load-extension=postgis --load-extension=h3
# extra files to remove in make clean
EXTRA_CLEAN += \
	$(LIBH3_DIR) \
	$(SQL_FULLINSTALL) \
	$(SQL_UPDATETEST) \
	src/extension.h \
	test/expected/install.out \
	test/expected/binding-functions.out \
	test/sql/binding-functions.sql \
	test/regression.diffs test/regression.out test/results \
	h3-*.zip

# PGXS boilerplate
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

# generate full installation sql (from uninstalled to latest)
$(SQL_FULLINSTALL): $(sort $(SQL_INSTALLS))
	cat $^ > $@
install: $(SQL_FULLINSTALL)

# build and compile h3 as static lib
$(LIBH3_DIR):
	git clone --branch $(LIBH3_VERSION) --depth 1 https://github.com/uber/h3 $(LIBH3_DIR)
$(LIBH3_DIR)/cmake: $(LIBH3_DIR)
	cd $(LIBH3_DIR) && cmake \
			-DCMAKE_C_FLAGS=-fPIC \
			-DCMAKE_INSTALL_PREFIX=./install \
			-DBUILD_TESTING=OFF \
			-DENABLE_COVERAGE=OFF \
			-DENABLE_DOCS=OFF \
			-DENABLE_FORMAT=OFF \
			-DENABLE_LINTING=OFF \
			.
$(LIBH3_FILE): $(LIBH3_DIR)/cmake
	cmake --build $(LIBH3_DIR) --target install
$(OBJS): $(LIBH3_FILE)

# generate header file with extension version baked in
src/extension.h: src/extension.in.h
	sed -e 's/@EXTVERSION@/$(EXTVERSION)/g' \
		$< > $@
$(OBJS): src/extension.h

# rules for testing the update path against full install
$(SQL_UPDATETEST): $(sort $(SQL_UPDATES))
	cat $^ > $@
#test/expected/install.out: $(SQL_UPDATES)
#	psql -c "DROP DATABASE IF EXISTS pg_regress;"
#	psql -c "CREATE DATABASE pg_regress;"
#	psql -d pg_regress -c "CREATE EXTENSION postgis;"
#	psql -d pg_regress -c "CREATE EXTENSION h3 VERSION 'updatetest';"
#	echo "\df h3*" > $@
#	psql -d pg_regress -c "\df h3*" >> $@
#	psql -c "DROP DATABASE pg_regress;"
install: $(SQL_UPDATETEST)
#installcheck: test/expected/install.out

# rules for checking we have the correct bindings
#$(LIBH3_DIR)/binding-functions: $(LIBH3_DIR)/cmake
#	cmake --build $(LIBH3_DIR) --target binding-functions
#test/expected/binding-functions.out: $(LIBH3_DIR)/binding-functions
#	psql -c "DROP DATABASE IF EXISTS pg_regress;"
#	psql -c "CREATE DATABASE pg_regress;"
#	echo "\echo '$(shell scripts/binding-functions.sh $(LIBH3_DIR))'" > $@
#	psql -d pg_regress -c "\echo '$(shell scripts/binding-functions.sh $(LIBH3_DIR))'" >> $@
#	psql -c "DROP DATABASE pg_regress;"
#test/sql/binding-functions.sql: test/expected/install.out scripts/extra-functions
#	psql -c "DROP DATABASE IF EXISTS pg_regress;"
#	psql -c "CREATE DATABASE pg_regress;"
#	echo "\echo '$(shell scripts/binding-functions.sh)'" > $@
#	psql -c "DROP DATABASE pg_regress;"
#installcheck: test/sql/binding-functions.sql test/expected/binding-functions.out
#$(OBJS): $(LIBH3_DIR)/binding-functions

# zip up for distribution
dist: clean
	git archive --prefix=h3-$(EXTVERSION)/ --output h3-dist.tar.gz HEAD
