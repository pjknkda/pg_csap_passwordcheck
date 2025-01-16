MODULE_big = csap_passwordcheck
OBJS = csap_passwordcheck.o
PGFILEDESC = "csap_passwordcheck - strengthen user password checks for CSAP"

REGRESS = csap_passwordcheck

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
