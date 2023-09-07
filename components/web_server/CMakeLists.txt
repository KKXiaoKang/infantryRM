idf_component_register(SRCS "webserver.c"
                    INCLUDE_DIRS "include"
                    EMBED_FILES "index.html"
                    PRIV_REQUIRES mqtt json nvs_flash Robot_Control)
                     