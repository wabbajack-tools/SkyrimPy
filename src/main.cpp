namespace
{
	void InitializeLog()
	{
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path) {
			util::report_and_fail("Failed to find standard logging directory"sv);
		}

		*path /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
		const auto level = spdlog::level::trace;
#else
		const auto level = spdlog::level::info;
#endif

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
		log->set_level(level);
		log->flush_on(level);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);
	}
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;

	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);

	v.UsesAddressLibrary(true);
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return v;
}();

void LogStatus(PyStatus* status)
{
    std::ofstream log;
    log.open("c:\\tmp\\log.txt", std::ios::app);
    log << status->err_msg << std::endl;
    log.flush();
    log.close();
}

void Log(const char* status)
{
    std::ofstream log;
    log.open("c:\\tmp\\log.txt", std::ios::app);
    log << status << std::endl;
    log.flush();
    log.close();
}



void init_python(void)
{
    PyStatus status;

    PyConfig config;
    Log("Starting Init");
    PyConfig_InitPythonConfig(&config);


    /* Set the program name. Implicitly preinitialize Python. */
    status = PyConfig_SetString(&config, &config.program_name,
                                L"E:\\SkyrimMods\\Skyrim\\SkyrimSE.exe");
    if (PyStatus_Exception(status)) {
        goto fail;
    }

    status = PyConfig_SetString(&config, &config.program_name,
                                L"E:\\SkyrimMods\\Skyrim\\SkyrimSE.exe");
    if (PyStatus_Exception(status)) {
        goto fail;
    }

    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) {
        goto fail;
    }
    PyConfig_Clear(&config);
    Log("Inited");
    return;

    fail:
    LogStatus(&status);
    PyConfig_Clear(&config);
    Py_ExitStatusException(status);
}


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
    InitializeLog();
    init_python();

    try {
        py::exec(R"(
with open("c:\\tmp\\foo.txt", "w") as f:
  f.write("dang")
)");
    }
    catch (std::exception ex) {
        Log(ex.what());
    }
    SKSE::Init(a_skse);
	return true;
}
