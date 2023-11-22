 #include "image.h"

const Kernel KERNEL_GAUSS = {
	COPY_BORDER,
	{
		{0.0625, 0.1250, 0.0625 },
		{0.1250, 0.2500, 0.1250 },
		{0.0625, 0.1250, 0.0625 },
	}, 
	0
};

const Kernel KERNEL_LAPLACE = {
	COPY_BORDER,
	{
		{0.0000,-1.0000, 0.0000 }, 
		{-1.000, 4.0000,-1.0000 }, 
		{0.0000,-1.0000, 0.0000 }, 
	}, 
	0
};

const Kernel KERNEL_HIGH = {
	COPY_BORDER,
	{
		{-1.000, -1.000, -1.000 }, 
		{-1.000,  8.000, -1.000 },
		{-1.000, -1.000, -1.000 }, 
	},
	0
};

const Kernel KERNEL_PREW_HX = {
	COPY_BORDER,
	{
		{-1.000, 0.0000, 1.0000 }, 
		{-1.000, 0.0000, 1.0000 }, 
		{-1.000, 0.0000, 1.0000 }, 
	},
	128
};

const Kernel KERNEL_PREW_HY = {
	COPY_BORDER,
	{
		{-1.000,-1.0000,-1.0000 }, 
		{0.0000, 0.0000, 0.0000 }, 
		{0.0000, 0.0000, 0.0000 },
	},
	128
};

const Kernel KERNEL_SOBEL_HX = {
	COPY_BORDER,
	{
		{-1.0000, 0.0000, 1.0000 }, 
		{-2.0000, 0.0000, 2.0000 }, 
		{-1.0000, 0.0000, 1.0000 },
	},
	128
};

const Kernel KERNEL_SOBEL_HY = {
	COPY_BORDER,
	{
		{-1.000,-2.0000,-1.0000 }, 
		{0.0000, 0.0000, 0.0000 }, 
		{1.0000, 2.0000, 1.0000 }, 
	},
	128
};

