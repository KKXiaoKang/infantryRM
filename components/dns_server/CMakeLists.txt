idf_component_register(SRCS "my_dns_server.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES mqtt json nvs_flash web_server Robot_Control)
                     