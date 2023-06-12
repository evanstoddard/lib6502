# Include file for using google test

# Include guard
include_guard()

# Include fetch
include(FetchContent)

# Fetch Google Test
message("Fetching Google Test...")
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Export googletest
FetchContent_MakeAvailable(googletest)