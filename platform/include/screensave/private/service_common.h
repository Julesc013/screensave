#ifndef SCREENSAVE_PRIVATE_SERVICE_COMMON_H
#define SCREENSAVE_PRIVATE_SERVICE_COMMON_H

#define SCREENSAVE_SERVICE_SEAM_FLAG_PRIVATE_ONLY     0x00000001UL
#define SCREENSAVE_SERVICE_SEAM_FLAG_DYNAMIC_OPTIONAL 0x00000002UL
#define SCREENSAVE_SERVICE_SEAM_FLAG_UNBOUND          0x00000004UL
#define SCREENSAVE_SERVICE_SEAM_FLAG_BOUND            0x00000008UL
#define SCREENSAVE_SERVICE_SEAM_FLAG_DISCOVERY_READY  0x00000010UL
#define SCREENSAVE_SERVICE_SEAM_FLAG_CURRENT_PRIMARY  0x00000020UL
#define SCREENSAVE_SERVICE_SEAM_FLAG_ADAPTER_SLOT     0x00000040UL

typedef struct screensave_service_binding_tag {
    const char *service_name;
    const char *provider_name;
    unsigned long flags;
    unsigned long capability_flags;
    unsigned long state_flags;
    void *service_state;
} screensave_service_binding;

#endif /* SCREENSAVE_PRIVATE_SERVICE_COMMON_H */
