#include "platform/CCDevice.h"
#include <stdio.h>
#include  "bcm_host.h"

NS_CC_BEGIN

int CCDevice::getDPI()
{
	static int dpi = -1;
	if (dpi == -1)
	{
	    // TODO: fix this....Hardcoded  640 witdh and 150mm screen area
            double xres = ((640 * 25.4) / (150));
            dpi = (int) (xres + 0.5);
	}
	return dpi;
}

NS_CC_END
