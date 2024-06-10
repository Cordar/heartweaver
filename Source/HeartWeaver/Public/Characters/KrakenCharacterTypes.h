#pragma once

UENUM(BlueprintType)
enum class EEmotionState : uint8 {
	ES_Neutral = 0,
	ES_Anger_Charge = 1,
	ES_Anger_Release = 2,
	ES_Fear_Charge = 3,
	ES_Fear_Release = 4,
	ES_Sadness_Charge = 5,
	ES_Sadness_Release = 6
};