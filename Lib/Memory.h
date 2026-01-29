#include <mach/mach.h>
#include <mach-o/dyld.h>
#include <CoreFoundation/CoreFoundation.h>
#include <string>

struct MemoryFileInfo {
    uint32_t index;
    const struct mach_header *header;
    const char *name;
    long long address;
};

MemoryFileInfo getBaseInfo() {
    MemoryFileInfo _info;
    std::string applicationsPath = "/private/var/containers/Bundle/Application";
    for (uint32_t i = 0; i < _dyld_image_count(); i++)
    {
        const char *name = _dyld_get_image_name(i);
        if (!name) continue;
        std::string fullpath(name);
        if (strncmp(fullpath.c_str(), applicationsPath.c_str(), applicationsPath.size()) == 0)
        {
            _info.index = i;
            _info.header = _dyld_get_image_header(i);
            _info.name = _dyld_get_image_name(i);
            _info.address = _dyld_get_image_vmaddr_slide(i);
            break;
        }
    }
    return _info;
}

///get baseinfo for UnityFramework.framework on application
MemoryFileInfo getBaseInfoByFrameworkName(const std::string& frameworkName) {
    MemoryFileInfo _info;
    const uint32_t imageCount = _dyld_image_count();
    for (uint32_t i = 0; i < imageCount; i++) {
        const char *name = _dyld_get_image_name(i);
        if (!name)
            continue;
        std::string fullpath(name);
        if (fullpath.length() < frameworkName.length() || fullpath.compare(fullpath.length() - frameworkName.length(), frameworkName.length(), frameworkName) != 0)
            continue;
        _info.index = i;
        _info.header = _dyld_get_image_header(i);
        _info.name = _dyld_get_image_name(i);
        _info.address = _dyld_get_image_vmaddr_slide(i);
        break;
    }
    return _info;
}

uintptr_t getAbsoluteAddress(uint64_t off, const char *frameworkName) {
    MemoryFileInfo info;
	if (frameworkName) {
		info = getBaseInfoByFrameworkName(frameworkName);
	}
	else {
		info = getBaseInfo();
	}
	
    if (info.address == 0)
        return 0;
    return info.address + off;
}

uint64_t getRealOffset(uint64_t offset){
    return getAbsoluteAddress(offset, "UnityFramework");
}