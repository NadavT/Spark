project(spark)

# Windows definitions
target_compile_definitions(spark PRIVATE VK_USE_PLATFORM_WIN32_KHR NOMINMAX)

# Spark Windows platform sources
target_sources(spark
    PRIVATE "window.cpp"
    PRIVATE "time.cpp"
    PRIVATE "input.cpp"
    )
