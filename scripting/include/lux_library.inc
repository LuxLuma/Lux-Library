/*  
*    Copyright (C) 2020  LuxLuma		acceliacat@gmail.com
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#if defined _lux_library_included
#endinput
#endif
#define _lux_library_included

#include <sourcemod>
#include <sdktools>

#define LUX_LIBRARY_VERSION "0.4.3"

#define GIMMEDATA "lux_library"

enum OS_Type
{
	OS_Unknown = -2,
	OS_invalid = -1,
	OS_windows = 0,
	OS_linux,
	OS_mac
}

static stock void GetGameData(Handle &hGamedata)
{
	hGamedata = LoadGameConfigFile(GIMMEDATA);
	if(hGamedata == null) 
		LogError("Failed to load \"%s.txt\" gamedata.", GIMMEDATA);
}

/*
*	win. nix, mac
*/
stock OS_Type GetOSType(Handle &hGamedata=null)
{
	static OS_Type os = OS_Unknown;
	if(os == OS_Unknown)
	{
		if(hGamedata == null)
		{
			GetGameData(hGamedata);
		}
		
		os = view_as<OS_Type>(GameConfGetOffset(hGamedata, "OS"));
		delete hGamedata;
	}
	return os;
}

/*
*	Get world space origin not all entities may support CollisionProperty for getting center (https://github.com/LuxLuma/l4d2_structs/blob/master/collision_property.h)
*/
stock void GetAbsOrigin(int iEntity, float vecOrigin[3], bool bCenter=false)
{
	GetEntPropVector(iEntity, Prop_Data, "m_vecAbsOrigin", vecOrigin);

	if(bCenter)
	{
		float vecMins[3];
		float vecMaxs[3];
		GetEntPropVector(iEntity, Prop_Send, "m_vecMins", vecMins);
		GetEntPropVector(iEntity, Prop_Send, "m_vecMaxs", vecMaxs);

		vecOrigin[0] += (vecMins[0] + vecMaxs[0]) * 0.5;
		vecOrigin[1] += (vecMins[1] + vecMaxs[1]) * 0.5;
		vecOrigin[2] += (vecMins[2] + vecMaxs[2]) * 0.5;
	}
}

/*
*	Smoothed between teleports clientside does not account for parented entities
*	use WORLDSPACE position. 
*/
stock bool SetAbsOrigin(int iEntity, const float vec[3])
{
	static Handle hSDKCall;
	if(hSDKCall == null)
	{
		Handle hGamedata;
		GetGameData(hGamedata);
		
		StartPrepSDKCall(SDKCall_Entity);
		if(PrepSDKCall_SetFromConf(hGamedata, SDKConf_Signature, "CBaseEntity::SetAbsOrigin"))
		{
			PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_Pointer);
			hSDKCall = EndPrepSDKCall();
			if(hSDKCall == null)
				LogError("Unable to prep SDKCall 'CBaseEntity::SetAbsOrigin'");
		}
		else
		{
			LogError("Error finding the 'CBaseEntity::SetAbsOrigin' signature.");
		}
		delete hGamedata;
		if(hSDKCall == null)
			return false;
	}
	SDKCall(hSDKCall, iEntity, vec);
	return true;
}

/*
*	Does not account for parented entities
*/
stock bool SetAbsVelocity(int iEntity, const float vec[3])
{
	static Handle hSDKCall;
	if(hSDKCall == null)
	{
		Handle hGamedata;
		GetGameData(hGamedata);
		
		StartPrepSDKCall(SDKCall_Entity);
		if(PrepSDKCall_SetFromConf(hGamedata, SDKConf_Signature, "CBaseEntity::SetAbsVelocity"))
		{
			PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_Pointer);
			hSDKCall = EndPrepSDKCall();
			if(hSDKCall == null)
				LogError("Unable to prep SDKCall 'CBaseEntity::SetAbsVelocity'");
		}
		else
		{
			LogError("Error finding the 'CBaseEntity::SetAbsVelocity' signature.");
		}
		delete hGamedata;
		if(hSDKCall == null)
			return false;
	}
	SDKCall(hSDKCall, iEntity, vec);
	return true;
}

/*
*	Does not account for parented entities
*/
stock bool SetAbsAngles(int iEntity, const float vec[3])
{
	static Handle hSDKCall;
	if(hSDKCall == null)
	{
		Handle hGamedata;
		GetGameData(hGamedata);
		
		StartPrepSDKCall(SDKCall_Entity);		
		if(PrepSDKCall_SetFromConf(hGamedata, SDKConf_Signature, "CBaseEntity::SetAbsAngles"))
		{
			PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_Pointer);
			hSDKCall = EndPrepSDKCall();
			if(hSDKCall == null)
				LogError("Unable to prep SDKCall 'CBaseEntity::SetAbsAngles'");
		}
		else
		{
			LogError("Error finding the 'CBaseEntity::SetAbsAngles' signature.");
		}
		delete hGamedata;
		if(hSDKCall == null)
			return false;
	}
	SDKCall(hSDKCall, iEntity, vec);
	return true;
}

/*
*	Bref copies back origin and angles vector data for world space attachment index location
*/
stock bool GetAttachmentVectors(int iEntity, char[] attachmentName, float vecOrigin[3], float vecAngles[3])
{
	int iAttachmentName = LookupAttachment(iEntity, attachmentName);
	if(iAttachmentName == 0)
		return false;
	GetAttachment(iEntity, iAttachmentName, vecOrigin, vecAngles);
	return true;
}

/*
*	Bref returns attachment index 0 upon failure
*/
stock int LookupAttachment(int iEntity, char[] attachmentName)
{
	static Handle hSDKCall;
	if(hSDKCall == null)
	{
		Handle hGamedata;
		GetGameData(hGamedata);
		
		StartPrepSDKCall(SDKCall_Entity);
		if(PrepSDKCall_SetFromConf(hGamedata, SDKConf_Signature, "CBaseAnimating::LookupAttachment"))
		{
			PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			hSDKCall = EndPrepSDKCall();
			if(hSDKCall == null)
				LogError("Unable to prep 'CBaseAnimating::LookupAttachment'");
		}
		else
		{
			LogError("Error finding the 'CBaseAnimating::LookupAttachment' signature.");
		}
		delete hGamedata;
		if(hSDKCall == null)
			return 0;
	}
	return SDKCall(hSDKCall, iEntity, attachmentName);
}


/*
*	Overloaded SDKCall->CBaseAnimating::GetAttachment(int a1, int a2, float *a3, float *a4)
*	Bref copies back origin and angles vector data for world space attachment location
*/
stock bool GetAttachment(int iEntity, int iAttachment, float vecOrigin[3], float vecAngles[3])
{
	static Handle hSDKCall;
	if(hSDKCall == null)
	{
		Handle hGamedata;
		GetGameData(hGamedata);
		
		StartPrepSDKCall(SDKCall_Entity);
		if(PrepSDKCall_SetFromConf(hGamedata, SDKConf_Signature, "CBaseAnimating::GetAttachment"))
		{
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef, _, VENCODE_FLAG_COPYBACK);
			PrepSDKCall_AddParameter(SDKType_QAngle, SDKPass_ByRef, _, VENCODE_FLAG_COPYBACK);
			hSDKCall = EndPrepSDKCall();
			if(hSDKCall == null)
				LogError("Unable to prep 'CBaseAnimating::GetAttachment'");
		}
		else
		{
			LogError("Error finding the 'CBaseAnimating::GetAttachment' signature.");
		}
		delete hGamedata;
		if(hSDKCall == null)
			return false;
	}
	SDKCall(hSDKCall, iEntity, iAttachment, vecOrigin, vecAngles);
	return true;
}

/*
*	FIXME make more reliable
*	Mostly works some brushes seem to have contents water valve -_-
*/
stock bool IsPositionInWater(float vecOrigin[3])
{
	Handle hTrace = TR_TraceRayEx(vecOrigin, vecOrigin, CONTENTS_WATER, RayType_EndPoint);
	bool bSolid = TR_StartSolid(hTrace);
	delete hTrace;
	return bSolid;
}

stock void Terror_SetAdrenalineTime(int client, float duration)
{
	// Get CountdownTimer address
	static int timerAddress = -1;
	if(timerAddress == -1)
	{
		timerAddress = FindSendPropInfo("CTerrorPlayer", "m_bAdrenalineActive") - 12;
	}
	
	//timerAddress + 4 = Duration
	//timerAddress + 8 = TimeStamp
	SetEntDataFloat(client, timerAddress + 4, duration);
	SetEntDataFloat(client, timerAddress + 8, GetGameTime() + duration);
	SetEntProp(client, Prop_Send, "m_bAdrenalineActive", (duration <= 0.0 ? 0 : 1), 1);
}

/*
*	return -1 for Adrenaline effect
*/
stock float Terror_GetAdrenalineTime(int client)
{
	// Get CountdownTimer address
	static int timerAddress = -1;
	if(timerAddress == -1)
	{
		timerAddress = FindSendPropInfo("CTerrorPlayer", "m_bAdrenalineActive") - 12;
	}
	
	//timerAddress + 8 = TimeStamp
	float time = GetEntDataFloat(client, timerAddress + 8);
	if(time <= GetGameTime())
		return -1.0;
	
	return time;
}

/*
*	NO LOS check more than 1 call perframe will not create more
*/
stock void PhysicsExplode(float vecPos[3], int iMagnitude, float flRadius, bool bDamage=false)
{
	static int iBoom = INVALID_ENT_REFERENCE;
	
	if(iBoom == INVALID_ENT_REFERENCE || !IsValidEntity(iBoom))
	{
		iBoom = EntIndexToEntRef(Entity_Create("env_physexplosion"));
		if(iBoom == INVALID_ENT_REFERENCE)
			return;
	
		DispatchKeyValue(iBoom, "spawnflags", "0");
		DispatchSpawn(iBoom);
	}
	
	if(bDamage)
	{
		DispatchKeyValue(iBoom, "spawnflags", "0");
	}
	else
	{
		DispatchKeyValue(iBoom, "spawnflags", "1");
	}
	
	static char sBuf[32];
	IntToString(iMagnitude, sBuf, sizeof(sBuf));
	DispatchKeyValue(iBoom, "magnitude", sBuf);
	
	IntToString(RoundFloat(flRadius), sBuf, sizeof(sBuf));
	DispatchKeyValue(iBoom, "radius", sBuf);
	DispatchKeyValueFloat(iBoom, "inner_radius", flRadius);

	TeleportEntity(iBoom, vecPos, NULL_VECTOR, NULL_VECTOR);

	AcceptEntityInput(iBoom, "Explode");
	RemoveEntity(iBoom);
}

//TEs

stock void TE_SetupExplodeForce(float vecPos[3], float flRadius, float flForce)
{
	static int iEffectIndex = INVALID_STRING_INDEX;
	if(iEffectIndex < 0)
	{
		iEffectIndex = __FindStringIndex2(FindStringTable("EffectDispatch"), "ExplosionForce");
		if(iEffectIndex == INVALID_STRING_INDEX)
			SetFailState("Unable to find EffectDispatch/ExplosionForce indexes");
		
	}
	
	TE_Start("EffectDispatch");
	TE_WriteNum("m_iEffectName", iEffectIndex);
	TE_WriteFloat("m_vOrigin.x", vecPos[0]);
	TE_WriteFloat("m_vOrigin.y", vecPos[1]);
	TE_WriteFloat("m_vOrigin.z", vecPos[2]);
	TE_WriteFloat("m_flRadius", flRadius);
	TE_WriteFloat("m_flMagnitude", flForce);
}


/*
*	iFlags = 1 autodespawn this is built into the model somehow, unsupported models instant despawn
*/
stock void TE_Create_PhysicsProp(float vecModelOrigin[3], 
								int iPrecacheModel, 
								float vecModelAngles[3]={0.0, ...}, 
								float vecVelocity[3]={0.0, ...}, 
								int iFlags=0,
								int iEffects=0,
								int iSkin=0,
								int RGB[3]={255, ...})
{
	TE_Start("physicsprop");
	TE_WriteVector("m_vecOrigin", vecModelOrigin);
	TE_WriteFloat("m_angRotation[0]", vecModelAngles[0]);
	TE_WriteFloat("m_angRotation[1]", vecModelAngles[1]);
	TE_WriteFloat("m_angRotation[2]", vecModelAngles[2]);
	TE_WriteVector("m_vecVelocity", vecVelocity);
	TE_WriteNum("m_nModelIndex", iPrecacheModel);
	TE_WriteNum("m_nFlags", iFlags);
	TE_WriteNum("m_nEffects", iEffects);
	TE_WriteNum("r", RGB[0]);
	TE_WriteNum("g", RGB[1]);
	TE_WriteNum("b", RGB[2]);
}

/*
*	Only 1 can exist clientside per tick.
*/
stock bool TE_SetupDynamicLight(float vecOrigin[3], int red, int green, int blue, float fRadius, float fTime, float fDecay=0.0, int exponent=0)
{
	static int iLastTick;
	int iCurrentTick = GetGameTickCount();
	
	if(iLastTick == iCurrentTick)
		return false;
	iLastTick = iCurrentTick;
	
	TE_Start("Dynamic Light");
	
	TE_WriteVector("m_vecOrigin", vecOrigin);
	TE_WriteNum("r", red);
	TE_WriteNum("g", green);
	TE_WriteNum("b", blue);
	TE_WriteNum("exponent", exponent);
	TE_WriteFloat("m_fRadius", fRadius);
	TE_WriteFloat("m_fTime", fTime);
	TE_WriteFloat("m_fDecay", fDecay);
	
	return true;
}

/*
*	Note only follows attachpoints cannot follow entity without an attachpoint, 
*	e.g. bullet casings are commonly emitted from attachpoints but don't follow.
*/
stock void TE_SetupParticleAttachment(int iParticleIndex, int iAttachmentIndex, int iEntIndex, bool bFollow=false)
{
	static float vecDummy[3]={0.0, 0.0, 0.0};
	static EngineVersion IsEngine;
	if(IsEngine == Engine_Unknown)
		IsEngine = GetEngineVersion();
	
	TE_Start("EffectDispatch");
	
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.x"	:"m_vOrigin[0]", vecDummy[0]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.y"	:"m_vOrigin[1]", vecDummy[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.z"	:"m_vOrigin[2]", vecDummy[2]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.x"	:"m_vStart[0]", vecDummy[0]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.y"	:"m_vStart[1]", vecDummy[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.z"	:"m_vStart[2]", vecDummy[2]);

	static int iEffectIndex = INVALID_STRING_INDEX;
	if(iEffectIndex < 0)
	{
		iEffectIndex = __FindStringIndex2(FindStringTable("EffectDispatch"), "ParticleEffect");
		if(iEffectIndex == INVALID_STRING_INDEX)
			SetFailState("Unable to find EffectDispatch/ParticleEffect indexes");
	}

	TE_WriteNum("m_iEffectName", iEffectIndex);
	TE_WriteNum("m_nHitBox", iParticleIndex);
	TE_WriteNum("entindex", iEntIndex);
	TE_WriteNum("m_nAttachmentIndex", iAttachmentIndex);
	TE_WriteNum("m_fFlags", 1);	//needed for attachments to work


	TE_WriteVector("m_vAngles", vecDummy);
	TE_WriteFloat("m_flMagnitude", 0.0);
	TE_WriteFloat("m_flScale", 1.0);
	TE_WriteFloat("m_flRadius", 0.0);
	
	if(IsEngine == Engine_Left4Dead2)
	{
		TE_WriteNum("m_nDamageType", bFollow ? 5 : 4);
	}
	else
	{
		TE_WriteNum("m_nDamageType", bFollow ? 4 : 3);
	}
}

/*
*	vecParticleEndPos required for bullet tracers and ropes for them to travel
*	WARNING looping particles will last forever with no way of removing them that i know of.
*/
stock void TE_SetupParticle(int iParticleIndex, float vecParticleStartPos[3], float vecParticleEndPos[3]={0.0, 0.0, 0.0}, float vecParticleAngles[3]={0.0, 0.0, 0.0})
{
	static EngineVersion IsEngine;
	if(IsEngine == Engine_Unknown)
		IsEngine = GetEngineVersion();
	
	TE_Start("EffectDispatch");
	
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.x"	:"m_vOrigin[0]", vecParticleStartPos[0]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.y"	:"m_vOrigin[1]", vecParticleStartPos[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.z"	:"m_vOrigin[2]", vecParticleStartPos[2]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.x"	:"m_vStart[0]", vecParticleEndPos[0]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.y"	:"m_vStart[1]", vecParticleEndPos[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.z"	:"m_vStart[2]", vecParticleEndPos[2]);

	static int iEffectIndex = INVALID_STRING_INDEX;
	if(iEffectIndex < 0)
	{
		iEffectIndex = __FindStringIndex2(FindStringTable("EffectDispatch"), "ParticleEffect");
		if(iEffectIndex == INVALID_STRING_INDEX)
			SetFailState("Unable to find EffectDispatch/ParticleEffect indexes");

	}

	TE_WriteNum("m_iEffectName", iEffectIndex);
	TE_WriteNum("m_nHitBox", iParticleIndex);

	TE_WriteNum("entindex", 0);
	TE_WriteNum("m_nAttachmentIndex", 0);
	TE_WriteNum("m_fFlags", 0);
	
	TE_WriteVector("m_vAngles", vecParticleAngles);
	
	TE_WriteFloat("m_flMagnitude", 0.0);
	TE_WriteFloat("m_flScale", 1.0);
	TE_WriteFloat("m_flRadius", 0.0);
	TE_WriteNum("m_nDamageType", 0);
}

/*
*	Follow entity origin
*/
stock void TE_SetupParticleFollowEntity(int iParticleIndex, int iEntIndex, float vecParticleAngles[3]={0.0, 0.0, 0.0})
{
	static float vecDummy[3]={0.0, 0.0, 0.0};
	static EngineVersion IsEngine;
	if(IsEngine == Engine_Unknown)
		IsEngine = GetEngineVersion();
	
	TE_Start("EffectDispatch");
	
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.x"	:"m_vOrigin[0]", vecDummy[0]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.y"	:"m_vOrigin[1]", vecDummy[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.z"	:"m_vOrigin[2]", vecDummy[2]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.x"	:"m_vStart[0]", vecDummy[0]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.y"	:"m_vStart[1]", vecDummy[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.z"	:"m_vStart[2]", vecDummy[2]);

	static int iEffectIndex = INVALID_STRING_INDEX;
	if(iEffectIndex < 0)
	{
		iEffectIndex = __FindStringIndex2(FindStringTable("EffectDispatch"), "ParticleEffect");
		if(iEffectIndex == INVALID_STRING_INDEX)
			SetFailState("Unable to find EffectDispatch/ParticleEffect indexes");
	}

	TE_WriteNum("m_iEffectName", iEffectIndex);
	TE_WriteNum("m_nHitBox", iParticleIndex);
	TE_WriteNum("entindex", iEntIndex);
	TE_WriteNum("m_nAttachmentIndex", 0);
	TE_WriteNum("m_fFlags", 1);


	TE_WriteVector("m_vAngles", vecParticleAngles);
	TE_WriteFloat("m_flMagnitude", 0.0);
	TE_WriteFloat("m_flScale", 1.0);
	TE_WriteFloat("m_flRadius", 0.0);
	
	TE_WriteNum("m_nDamageType", (IsEngine == Engine_Left4Dead2) ? 1 : 1);
}

/*
*	L4d2 only maintains offset from a world origin
*/
stock void TE_SetupParticleFollowEntity_MaintainOffset(int iParticleIndex, int iEntIndex, float vecParticleStartPos[3], float vecParticleAngles[3]={0.0, 0.0, 0.0})
{
	static float vecDummy[3]={0.0, 0.0, 0.0};
	static EngineVersion IsEngine;
	if(IsEngine == Engine_Unknown)
		IsEngine = GetEngineVersion();
	
	TE_Start("EffectDispatch");
	
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.x"	:"m_vOrigin[0]", vecParticleStartPos[0]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.y"	:"m_vOrigin[1]", vecParticleStartPos[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.z"	:"m_vOrigin[2]", vecParticleStartPos[2]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.x"	:"m_vStart[0]", vecDummy[0]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.y"	:"m_vStart[1]", vecDummy[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.z"	:"m_vStart[2]", vecDummy[2]);

	static int iEffectIndex = INVALID_STRING_INDEX;
	if(iEffectIndex < 0)
	{
		iEffectIndex = __FindStringIndex2(FindStringTable("EffectDispatch"), "ParticleEffect");
		if(iEffectIndex == INVALID_STRING_INDEX)
			SetFailState("Unable to find EffectDispatch/ParticleEffect indexes");
	}

	TE_WriteNum("m_iEffectName", iEffectIndex);
	TE_WriteNum("m_nHitBox", iParticleIndex);
	TE_WriteNum("entindex", iEntIndex);
	TE_WriteNum("m_nAttachmentIndex", 0);
	TE_WriteNum("m_fFlags", 1);


	TE_WriteVector("m_vAngles", vecParticleAngles);
	TE_WriteFloat("m_flMagnitude", 0.0);
	TE_WriteFloat("m_flScale", 1.0);
	TE_WriteFloat("m_flRadius", 0.0);
	
	TE_WriteNum("m_nDamageType", 3);
}

/*
*	Follow entity origin
*/
stock bool TE_SetupParticleFollowEntity_Name(char[] sParticleName, int iEntIndex, float vecParticleAngles[3]={0.0, 0.0, 0.0})
{
	int iParticleStringIndex = GetParticleIndex(sParticleName);
	if(iParticleStringIndex == INVALID_STRING_INDEX)
	{
		return false;
	}
	TE_SetupParticleFollowEntity(iParticleStringIndex, iEntIndex, vecParticleAngles);
	return true;
}

/*
*	L4d2 only maintains offset from a world origin of the parent, not reliable since this is not a server side entity so lag can make offset be off.
*/
stock bool TE_SetupParticleFollowEntity_MaintainOffset_Name(char[] sParticleName, int iEntIndex, float vecParticleStartPos[3], float vecParticleAngles[3]={0.0, 0.0, 0.0})
{
	int iParticleStringIndex = GetParticleIndex(sParticleName);
	if(iParticleStringIndex == INVALID_STRING_INDEX)
	{
		return false;
	}
	TE_SetupParticleFollowEntity_MaintainOffset(iParticleStringIndex, iEntIndex, vecParticleStartPos, vecParticleAngles);
	return true;
}

stock bool TE_SetupParticle_Name(char[] sParticleName, float vecParticleStartPos[3], float vecParticleEndPos[3]={0.0, 0.0, 0.0}, float vecParticleAngles[3]={0.0, 0.0, 0.0})
{
	int iParticleStringIndex = GetParticleIndex(sParticleName);
	if(iParticleStringIndex == INVALID_STRING_INDEX)
	{
		return false;
	}
	TE_SetupParticle(iParticleStringIndex, vecParticleStartPos, vecParticleEndPos, vecParticleAngles);
	return true;
}

/*
*	Note only follows attachpoints cannot follow entity without an attachpoint, 
*	e.g. bullet casings are commonly emitted from attachpoints but don't follow.
*/
stock bool TE_SetupParticleAttachment_Names(char[] sParticleName, char[] sAttachmentName, int iEntIndex, bool bFollow=false)
{
	int iParticleStringIndex = GetParticleIndex(sParticleName);
	if(iParticleStringIndex == INVALID_STRING_INDEX)
	{
		return false;
	}
	int iAttachmentIndex = LookupAttachment(iEntIndex, sAttachmentName);
	TE_SetupParticleAttachment(iParticleStringIndex, iAttachmentIndex, iEntIndex, bFollow);
	return true;
}

/*
*	Stops all particles emitted on this entity, e.g. attachment followers.
*	No idea how to make it target particles.
*/
stock bool TE_SetupStopAllParticles(int iEntIndex)
{
	static float vecDummy[3]={0.0, 0.0, 0.0};
	static EngineVersion IsEngine;
	if(IsEngine == Engine_Unknown)
		IsEngine = GetEngineVersion();
	
	TE_Start("EffectDispatch");
	
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.x"	:"m_vOrigin[0]", vecDummy[0]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.y"	:"m_vOrigin[1]", vecDummy[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.z"	:"m_vOrigin[2]", vecDummy[2]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.x"	:"m_vStart[0]", vecDummy[0]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.y"	:"m_vStart[1]", vecDummy[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.z"	:"m_vStart[2]", vecDummy[2]);

	static int iEffectIndex = INVALID_STRING_INDEX;
	if(iEffectIndex < 0)
	{
		iEffectIndex = __FindStringIndex2(FindStringTable("EffectDispatch"), "ParticleEffectStop");
		if(iEffectIndex == INVALID_STRING_INDEX)
			SetFailState("Unable to find EffectDispatch/ParticleEffectStop indexes");
	}

	TE_WriteNum("m_iEffectName", iEffectIndex);
	TE_WriteNum("m_nHitBox", 0);

	TE_WriteNum("entindex", iEntIndex);
	TE_WriteNum("m_nAttachmentIndex", 0);
	TE_WriteNum("m_fFlags", 1);
	TE_WriteVector("m_vAngles", vecDummy);
	TE_WriteFloat("m_flMagnitude", 0.0);
	TE_WriteFloat("m_flScale", 0.0);
	TE_WriteFloat("m_flRadius", 0.0);
	TE_WriteNum("m_nDamageType", 0);
}


/*
*	Make use of clientside tracer sound, uses clientside ray from 2 points to play wizz sound
*/
stock void TE_SetupTracerSound(float vecParticleStartPos[3], float vecParticleEndPos[3])
{
	static float vecDummy[3]={0.0, 0.0, 0.0};
	static EngineVersion IsEngine;
	if(IsEngine == Engine_Unknown)
		IsEngine = GetEngineVersion();
	
	TE_Start("EffectDispatch");
	
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.x"	:"m_vOrigin[0]", vecParticleStartPos[0]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.y"	:"m_vOrigin[1]", vecParticleStartPos[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.z"	:"m_vOrigin[2]", vecParticleStartPos[2]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.x"	:"m_vStart[0]", vecParticleEndPos[0]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.y"	:"m_vStart[1]", vecParticleEndPos[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.z"	:"m_vStart[2]", vecParticleEndPos[2]);

	static int iEffectIndex = INVALID_STRING_INDEX;
	if(iEffectIndex < 0)
	{
		iEffectIndex = __FindStringIndex2(FindStringTable("EffectDispatch"), "TracerSound");
		if(iEffectIndex == INVALID_STRING_INDEX)
			SetFailState("Unable to find EffectDispatch/TracerSound indexes");

	}

	TE_WriteNum("m_iEffectName", iEffectIndex);
	TE_WriteNum("m_nHitBox", 0);

	TE_WriteNum("entindex", 0);
	TE_WriteNum("m_nAttachmentIndex", 0);
	
	TE_WriteNum("m_fFlags", 1);
	
	TE_WriteVector("m_vAngles", vecDummy);
	
	TE_WriteFloat("m_flMagnitude", 0.0);
	TE_WriteFloat("m_flScale", 0.0);
	TE_WriteFloat("m_flRadius", 0.0);
	TE_WriteNum("m_nDamageType", 0);
}

static stock int GetParticleIndex(char[] sParticleName)
{
	static int iParticleTableid = INVALID_STRING_TABLE;
	if(iParticleTableid == INVALID_STRING_TABLE)
	{
		iParticleTableid = FindStringTable("ParticleEffectNames");
		if(iParticleTableid == INVALID_STRING_TABLE)
			SetFailState("Failed to find 'ParticleEffectNames' stringtable.");
	}
	
	int iParticleStringIndex = __FindStringIndex2(iParticleTableid, sParticleName);
	if(iParticleStringIndex == INVALID_STRING_INDEX)
	{
		iParticleStringIndex = __PrecacheParticleSystem(sParticleName);
		if(iParticleStringIndex == INVALID_STRING_INDEX)
		{
			LogError("Unable to late precache of particle '%s'", sParticleName);
		}
		else
		{
			LogError("Late precache of particle '%s'", sParticleName);
		}
	}
	return iParticleStringIndex;
}

//Credit smlib https://github.com/bcserv/smlib
/*
 * Rewrite of FindStringIndex, because in my tests
 * FindStringIndex failed to work correctly.
 * Searches for the index of a given string in a string table. 
 * 
 * @param tableidx		A string table index.
 * @param str			String to find.
 * @return				String index if found, INVALID_STRING_INDEX otherwise.
 */
static stock int __FindStringIndex2(int tableidx, const char[] str)
{
	static char buf[1024];

	int numStrings = GetStringTableNumStrings(tableidx);
	for (int i=0; i < numStrings; i++) {
		ReadStringTable(tableidx, i, buf, sizeof(buf));
		
		if (StrEqual(buf, str)) {
			return i;
		}
	}
	
	return INVALID_STRING_INDEX;
}

//Credit smlib https://github.com/bcserv/smlib
/*
 * Precaches the given particle system.
 * It's best to call this OnMapStart().
 * Code based on Rochellecrab's, thanks.
 *
 * @param particleSystem	Name of the particle system to precache.
 * @return					Returns the particle system index, INVALID_STRING_INDEX on error.
 */
static stock int __PrecacheParticleSystem(const char[] particleSystem)
{
	static int particleEffectNames = INVALID_STRING_TABLE;

	if (particleEffectNames == INVALID_STRING_TABLE) {
		if ((particleEffectNames = FindStringTable("ParticleEffectNames")) == INVALID_STRING_TABLE) {
			return INVALID_STRING_INDEX;
		}
	}

	int index = __FindStringIndex2(particleEffectNames, particleSystem);
	if (index == INVALID_STRING_INDEX) 
	{
		int numStrings = GetStringTableNumStrings(particleEffectNames);
		if (numStrings >= GetStringTableMaxStrings(particleEffectNames)) 
		{
			return INVALID_STRING_INDEX;
		}

		AddToStringTable(particleEffectNames, particleSystem);
		index = numStrings;
	}

	return index;
}

/*
*	Wrapper for smlib's PrecacheParticleSystem to avoid include collisions
*/
stock int Precache_Particle_System(const char[] particleSystem)
{
	return __PrecacheParticleSystem(particleSystem);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Legacy Particle Stock///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*	iParticleIndex = "ParticleString" index location in String table "ParticleEffectNames"
*	iEntIndex = entity index usually used for attachpoints
*	fDelay = delay for TE_SendToAll
*	SendToAll = if send to all false call send to clients your self
*	sParticleName =  particle name only used if iParticleIndex -1 it will find the index for you
*	iAttachmentIndex =  attachpoint index there is no way to get this currently with sm, gotta decompile the model :p
*	ParticleAngles =  angles usually effects particles that have no gravity
*	iFlags = 1 required for attachpoints as well as damage type ^^
*	iDamageType = saw it being used in impact effect dispatch and attachpoints need to be set to use (maybe)
*	fMagnitude = no idea saw being used with pipebomb blast (needs testing)
*	fScale = guess its particle scale but most dont scale (needs testing)
*/
stock bool L4D_TE_Create_Particle(float fParticleStartPos[3]={0.0, 0.0, 0.0}, 
								float fParticleEndPos[3]={0.0, 0.0, 0.0}, 
								int iParticleIndex=-1, 
								int iEntIndex=0,
								float fDelay=0.0,
								bool SendToAll=true,
								char sParticleName[64]="",
								int iAttachmentIndex=0,
								float fParticleAngles[3]={0.0, 0.0, 0.0}, 
								int iFlags=0,
								int iDamageType=0,
								float fMagnitude=0.0,
								float fScale=1.0,
								float fRadius=0.0)
{
	TE_Start("EffectDispatch");
	
	static EngineVersion IsEngine;
	if(IsEngine == Engine_Unknown)
		IsEngine = GetEngineVersion();
	
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.x"	:"m_vOrigin[0]", fParticleStartPos[0]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.y"	:"m_vOrigin[1]", fParticleStartPos[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.z"	:"m_vOrigin[2]", fParticleStartPos[2]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.x"	:"m_vStart[0]", fParticleEndPos[0]);//end point usually for bulletparticles or ropes
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.y"	:"m_vStart[1]", fParticleEndPos[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.z"	:"m_vStart[2]", fParticleEndPos[2]);
	
	static int iEffectIndex = INVALID_STRING_INDEX;
	if(iEffectIndex < 0)
	{
		iEffectIndex = __FindStringIndex2(FindStringTable("EffectDispatch"), "ParticleEffect");
		if(iEffectIndex == INVALID_STRING_INDEX)
			SetFailState("Unable to find EffectDispatch/ParticleEffect indexes");
		
	}
	
	TE_WriteNum("m_iEffectName", iEffectIndex);
	
	if(iParticleIndex < 0)
	{
		static int iParticleStringIndex = INVALID_STRING_INDEX;
		iParticleStringIndex = __FindStringIndex2(FindStringTable("ParticleEffectNames"), sParticleName);
		if(iParticleStringIndex == INVALID_STRING_INDEX)
			return false;
		
		TE_WriteNum("m_nHitBox", iParticleStringIndex);
	}
	else
		TE_WriteNum("m_nHitBox", iParticleIndex);
	
	TE_WriteNum("entindex", iEntIndex);
	TE_WriteNum("m_nAttachmentIndex", iAttachmentIndex);
	
	TE_WriteVector("m_vAngles", fParticleAngles);
	
	TE_WriteNum("m_fFlags", iFlags);
	TE_WriteFloat("m_flMagnitude", fMagnitude);// saw this being used in pipebomb needs testing what it does probs shaking screen?
	TE_WriteFloat("m_flScale", fScale);
	TE_WriteFloat("m_flRadius", fRadius);// saw this being used in pipebomb needs testing what it does probs shaking screen?
	TE_WriteNum("m_nDamageType", iDamageType);// this shit is required dunno why for attachpoint emitting valve probs named it wrong
	
	if(SendToAll)
		TE_SendToAll(fDelay);
	
	return true;
}

stock bool L4D_TE_Stop_Particle(float fParticleStartPos[3]={0.0, 0.0, 0.0},
								float fParticleEndPos[3]={0.0, 0.0, 0.0},
								int iParticleIndex=-1,
								int iEntIndex=0,
								float fDelay=0.0,
								bool SendToAll=true,
								char sParticleName[64]="",
								int iAttachmentIndex=0,
								float fParticleAngles[3]={0.0, 0.0, 0.0},
								int iFlags=0,
								int iDamageType=0,
								float fMagnitude=0.0,
								float fScale=1.0,
								float fRadius=0.0)
{
	TE_Start("EffectDispatch");
	
	static EngineVersion IsEngine;
	if(IsEngine == Engine_Unknown)
		IsEngine = GetEngineVersion();
	
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.x"	:"m_vStart[0]", fParticleStartPos[0]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.y"	:"m_vStart[1]", fParticleStartPos[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vOrigin.z"	:"m_vStart[2]", fParticleStartPos[2]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.x"	:"m_vOrigin[0]", fParticleEndPos[0]);//end point usually for bulletparticles or ropes
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.y"	:"m_vOrigin[1]", fParticleEndPos[1]);
	TE_WriteFloat(IsEngine == Engine_Left4Dead2 ? "m_vStart.z"	:"m_vOrigin[2]", fParticleEndPos[2]);

	static int iEffectIndex = INVALID_STRING_INDEX;
	if(iEffectIndex < 0)
	{
		iEffectIndex = FindStringIndex2(FindStringTable("EffectDispatch"), "ParticleEffectStop");
		if(iEffectIndex == INVALID_STRING_INDEX)
			SetFailState("Unable to find EffectDispatch/ParticleEffectStop indexes");

	}

	TE_WriteNum("m_iEffectName", iEffectIndex);

	if(iParticleIndex < 0)
	{
		static int iParticleStringIndex = INVALID_STRING_INDEX;
		iParticleStringIndex = __FindStringIndex2(FindStringTable("ParticleEffectNames"), sParticleName);
		if(iParticleStringIndex == INVALID_STRING_INDEX)
			return false;

		TE_WriteNum("m_nHitBox", iParticleStringIndex);
	}
	else
		TE_WriteNum("m_nHitBox", iParticleIndex);

	TE_WriteNum("entindex", iEntIndex);
	TE_WriteNum("m_nAttachmentIndex", iAttachmentIndex);

	TE_WriteVector("m_vAngles", fParticleAngles);

	TE_WriteNum("m_fFlags", iFlags);
	TE_WriteFloat("m_flMagnitude", fMagnitude);// saw this being used in pipebomb needs testing what it does probs shaking screen?
	TE_WriteFloat("m_flScale", fScale);
	TE_WriteFloat("m_flRadius", fRadius);// saw this being used in pipebomb needs testing what it does probs shaking screen?
	TE_WriteNum("m_nDamageType", iDamageType);// this shit is required dunno why for attachpoint emitting valve probs named it wrong

	if(SendToAll)
		TE_SendToAll(fDelay);

	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////