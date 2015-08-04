#include "Statistics.h"


Statistics::Statistics(void)
{
}


Statistics::~Statistics(void)
{
}

void Statistics::saveConfig(String filename)
{
	json_t* root = json_object();
	json_t* stats = json_object();
	json_object_set(root, "statistics", stats);

	json_t* totals = json_object();
	json_object_set(root, "totals", totals);
	json_t* helper = json_integer(totalActions.deaths);
	json_object_set(totals, "deaths", helper);
	helper = json_integer(totalActions.longJumps);
	json_object_set(totals, "long-jumps", helper);
	helper = json_integer(totalActions.normalJumps);
	json_object_set(totals, "normal-jumps", helper);
	helper = json_integer(totalActions.longWallJumps);
	json_object_set(totals, "long-wall-jumps", helper);
	helper = json_integer(totalActions.normalWallJumps);
	json_object_set(totals, "normal-wall-jumps", helper);
	helper = json_real(totalActions.distanceAir);
	json_object_set(totals, "distance-air", helper);
	helper = json_real(totalActions.distanceWalking);
	json_object_set(totals, "distance-walking", helper);

	json_t* score = json_object();
	for (int i = 0; i < sizeof(scoreHistory) / sizeof(scoreHistory[0]); i++)
	{
		// TODO load score stats
		json_t* arrayHelper = json_array();
	}

	json_object_set(totals, "y", helper);
	json_dump_file(root, filename.c_str(), JSON_INDENT(2) & JSON_SORT_KEYS & JSON_PRESERVE_ORDER & JSON_ENCODE_ANY);
}

void Statistics::loadConfig(String)
{
}