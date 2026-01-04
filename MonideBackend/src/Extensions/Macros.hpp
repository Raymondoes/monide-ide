#define MONIDE_MODULE_MAIN()                   \
    int main(int argc, char **argv);           \
    int ModuleMainImpl(int argc, char **argv); \
    int main(int argc, char **argv)            \
    {                                          \
        return ModuleMainImpl(argc, argv);     \
    }                                          \
    int ModuleMainImpl(int argc, char **argv)

#define MONIDE_MODULE_INFO(name, version, author, description, apiVersion, entryPoint) \ extern "C" __attribute__((visibility("default"))) Monide::ModuleInfo ModuleInfo = { \
                                                                                           name,                                                                             \
                                                                                           version,                                                                          \
                                                                                           author,                                                                           \
                                                                                           description,                                                                      \
                                                                                           apiVersion,                                                                       \
                                                                                           sizeof(ModuleInfo),                                                               \
                                                                                           entryPoint};

/*
    MONIDE_MODULE_MAIN()
    {
        // Module code here
        return 0;
    }
*/