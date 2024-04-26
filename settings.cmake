
include_guard(GLOBAL)

set(project_dir "${CMAKE_CURRENT_LIST_DIR}")
file(GLOB project_modules ${project_dir}/projects/*)
list(
    APPEND
        CMAKE_MODULE_PATH
	${project_dir}/seL4
        ${project_dir}/seL4_tools/cmake-tool/helpers/
        ${project_dir}/seL4_tools/elfloader-tool/
	${project_modules}
)

set(SEL4_CONFIG_DEFAULT_ADVANCED ON)

include(application_settings)

set(KernelArmExportPCNTUser ON CACHE BOOL "Export physical counter to EL0")
set(KernelArmExportPTMRUser ON CACHE BOOL "Export physical timer to EL0")
set(KernelArmHypervisorSupport ON CACHE BOOL "" FORCE)
set(KernelRootCNodeSizeBits 18 CACHE STRING "")
set(KernelArch "arm" CACHE STRING "" FORCE)
set(KernelSel4Arch "aarch64" CACHE STRING "")
set(PLATFORM "qemu-arm-virt" CACHE STRING "Platform to test")
set(ARM_CPU "cortex-a72" CACHE STRING "")
set(KernelArmVtimerUpdateVOffset OFF CACHE BOOL "" FORCE)
set(KernelArmDisableWFIWFETraps ON CACHE BOOL "" FORCE)
set(KernelNumDomains 1 CACHE STRING "" FORCE)
set(KernelMaxNumNodes 1 CACHE STRING "" FORCE)
set(KernelIsMCS OFF CACHE BOOL "" FORCE)
set(KernelIOMMU ON CACHE BOOL "" FORCE)
set(LibUSB OFF CACHE BOOL "" FORCE)
set(QEMU_MEMORY "4096")
#set(LibSel4TestPrinterRegex ".*" CACHE STRING "A POSIX regex pattern used to filter tests")
#set(LibSel4TestPrinterHaltOnTestFailure OFF CACHE BOOL "Halt on the first test failure")
#set(LibSel4MuslcSysMorecoreBytes 0 CACHE STRING "" FORCE)
#mark_as_advanced(CLEAR LibSel4TestPrinterRegex LibSel4TestPrinterHaltOnTestFailure)

correct_platform_strings()
find_package(seL4 REQUIRED)
sel4_configure_platform_settings()

set(HardwareDebugAPI ON CACHE BOOL "" FORCE)
set(LibSel4PlatSupportUseDebugPutChar true CACHE BOOL "" FORCE)
#set(LibSel4MuslcSysDebugHalt FALSE CACHE BOOL "" FORCE)
ApplyCommonReleaseVerificationSettings(FALSE FALSE)
#set(LibSel4MuslcSysDebugHalt FALSE CACHE BOOL "" FORCE)
#set(LibSel4TestPrintXML OFF CACHE BOOL "" FORCE)

macro(common_setup_environment)
    
    find_package(elfloader-tool REQUIRED)
    find_package(musllibc REQUIRED)
    find_package(util_libs REQUIRED)
    find_package(seL4_libs REQUIRED)

    sel4_import_kernel()
    elfloader_import_project()

    musllibc_setup_build_environment_with_sel4runtime()
    sel4_import_libsel4()
    util_libs_import_libraries()
    sel4_libs_import_libraries()

endmacro()
