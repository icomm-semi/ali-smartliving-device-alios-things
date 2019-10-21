#ifndef BSP_H
#define BSP_H

#include <attrs.h>
uint32_t system_us2delaytick(uint32_t us);
void system_delay(uint32_t delaytick) ATTRIBUTE_SECTION_FBOOT;

#endif /* end of include guard: BSP_H */
