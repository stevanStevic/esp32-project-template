idf_component_register(
    SRCS
        "main.cpp"
        "src/app.cpp"
        "src/components/logger.cpp"
        # TODO: Add new component sources here as the project grows, e.g.:
        #   "src/components/my_component.cpp"
    INCLUDE_DIRS
        "include")
