#pragma once

#include "./OPengine.h"

void SceneAddLoader();

struct SceneDataModelMaterial {
	OPchar* texture = NULL;
};

struct SceneDataModel {
	OPchar* name = NULL;
	OPchar* opm = NULL;
	OPchar* type = NULL;
	OPvec3 position = OPVEC3_ZERO;
	OPvec3 scale = OPVEC3_ONE;
	OPvec3 rotation = OPVEC3_ZERO;
	OPchar* gametype = NULL;
	OPboundingBox3D boundingBox;
	OPvec3 boundingBoxPosition = OPVEC3_ZERO;
	OPchar* userData = NULL;
	SceneDataModelMaterial material;
	ui32 childrenCount = 0;
	SceneDataModel* children = NULL;
};

struct SceneData {
	ui32 modelsCount;
	SceneDataModel* models = NULL;
};

#include "jsmn.h"

struct TokenReader {

	const char* _json;
	char _buffer[8192];
	jsmntok_t _tokens[8192];
	jsmn_parser _p;

	ui32 _ind;
	ui32 _tokenCount;

	TokenReader(const char* json, ui32 length) {
		_json = json;
		jsmn_init(&_p);
		_tokenCount = jsmn_parse(&_p, json, length, _tokens, 8192);

		if (_tokenCount < 0) {
			OPlogErr("Failed to parse JSON: %d\n", _tokenCount);
		}

		_ind = 0;

		/* Assume the top-level element is an object */
		if (_tokenCount < 1 || _tokens[_ind].type != JSMN_OBJECT) {
			OPlogErr("Object expected\n");
		}
	}

	ui32 Size() {
		return _tokens[_ind].size;
	}

	i32 I32() {
		jsmntok_t *num = &_tokens[_ind];
		int j = 0;
		for (j = 0; j < num->end - num->start; j++) {
			_buffer[j] = _json[num->start + j];
		}
		_buffer[j] = NULL;

		return atoi(_buffer);
	}

	f32 F32() {
		jsmntok_t *num = &_tokens[_ind];
		int j = 0;
		for (j = 0; j < num->end - num->start; j++) {
			_buffer[j] = _json[num->start + j];
		}
		_buffer[j] = NULL;

		return atof(_buffer);
	}

	const OPchar* GetString() {
		jsmntok_t *num = &_tokens[_ind];
		int j = 0;
		for (j = 0; j < num->end - num->start; j++) {
			_buffer[j] = _json[num->start + j];
		}
		_buffer[j] = NULL;
		return _buffer;
	}

	OPchar* CopyString() {
		return OPstringCopy(GetString());
	}

	bool Equals(const OPchar* str) {
		jsmntok_t *tok = &_tokens[_ind];

		if (tok->type == JSMN_STRING && (int)strlen(str) == tok->end - tok->start &&
			strncmp(_json + tok->start, str, tok->end - tok->start) == 0) {
			return true;
		}
		return false;
	}

	bool IsArray() {
		jsmntok_t *tok = &_tokens[_ind];
		if (tok->type == JSMN_ARRAY) {
			return true;
		}
		return false;
	}

	bool IsObject() {
		jsmntok_t *tok = &_tokens[_ind];
		if (tok->type == JSMN_OBJECT) {
			return true;
		}
		return false;
	}

	OPvec3 GetVec3() {
		jsmntok_t *g = &_tokens[_ind + 1];
		OPvec3 data;

		if (g->type != JSMN_ARRAY) {
			return OPVEC3_ZERO;
		}

		for (int i = 0; i < g->size; i++) {
			jsmntok_t *num = &_tokens[_ind + i + 2];
			int j = 0;
			for (j = 0; j < num->end - num->start; j++) {
				_buffer[j] = _json[num->start + j];
			}
			_buffer[j] = NULL;

			data[i] = atof(_buffer);
		}
		return data;
	}

	void NextClear() {
		Next();
		if (IsArray() || IsObject()) {
			ui32 s = Size();
			for (ui32 i = 0; i < s; i++) {
				Next();
			}
		}
	}

	void NextClearArray() {
		ui32 tokens = Size();
		Print();

		for (ui32 i = 0; i < tokens; i++) {
			Next();
			Print();

			if (IsArray()) {
				NextClearArray();
			}
			else if (IsObject()) {
				NextClearObject();
			}
		}
	}

	void NextClearObject() {
		ui32 tokens = Size();
		Print();

		for (ui32 i = 0; i < tokens; i++) {
			Next();
			Print();

			Next();
			Print();

			if (IsArray()) {
				NextClearArray();
			} else if(IsObject()) {
				NextClearObject();
			}
		}
	}

	void Next() {
		_ind++;
	}

	void Next(ui32 n) {
		_ind+=n;
	}

	void Print() {
		jsmntok_t *g = &_tokens[_ind + 1];

		if (g->type == JSMN_ARRAY) {
			OPlogInfo("Array: %s", GetString());
		}
		else if (g->type == JSMN_PRIMITIVE) {
			OPlogInfo("%i", I32());
		}
		else {
			OPlogInfo("Token: %s", GetString());
		}
	}
};