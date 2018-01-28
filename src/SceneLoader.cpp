#include "SceneLoader.h"

SceneDataModelMaterial ReaderModelMaterial(TokenReader& reader) {
	SceneDataModelMaterial result = SceneDataModelMaterial();

	ui32 totalKeys = reader.Size();

	for (i32 i = 0; i < totalKeys; i++) {
		reader.Next();
		OPlog("%s", reader.GetString());

		if (reader.Equals("texture")) {
			reader.Next();
			result.texture = reader.CopyString();
			if (result.texture == NULL) {
				OPlogErr("Texture was not valid");
			}
		}
		else {
			reader.NextClear();
		}
	}

	return result;
}

OPboundingBox3D ReadBounds(TokenReader& reader) {
	OPboundingBox3D result;

	ui32 totalKeys = reader.Size();
	for (i32 i = 0; i < totalKeys; i++) {
		reader.Next();

		if (reader.Equals("min")) {
			result.min = reader.GetVec3();
			reader.Next(4);
		}
		else if (reader.Equals("max")) {
			result.max = reader.GetVec3();
			reader.Next(4);
		}
	}

	return result;
}

SceneDataModel LoadModel(TokenReader& reader) {
	SceneDataModel data;

	ui32 totalKeys = reader.Size();
	// OPlogInfo(reader.GetString());
	for (i32 i = 0; i < totalKeys; i++) {
		reader.Next();
		OPlogInfo("%s", reader.GetString());

		if (reader.Equals("name")) {
			reader.Next();
			data.name = reader.CopyString();
			if(OPstringEquals(data.name, "GearButton")) {
				OPlogInfo("GearButton");
			}
		}
		else if(reader.Equals("opm")) {
			reader.Next();
			data.opm = reader.CopyString();
		}
		else if(reader.Equals("type")) {
			reader.Next();
			data.type = reader.CopyString();
		}
		else if (reader.Equals("position")) {
			data.position = reader.GetVec3();
			reader.Next(4);
		}
		else if (reader.Equals("scale")) {
			data.scale = reader.GetVec3();
			reader.Next(4);
		}
		else if (reader.Equals("rotation")) {
			data.rotation = reader.GetVec3();
			reader.Next(4);
		}
		else if (reader.Equals("gameType")) {
			reader.Next();
			data.gametype = reader.CopyString();
		}
		else if (reader.Equals("userData")) {
			reader.Next();
			data.userData = reader.CopyString();
			reader.NextClearObject();
		}
		else if (reader.Equals("boundingBox")) {
			reader.Next();
			data.boundingBox = ReadBounds(reader);
		}
		else if (reader.Equals("material")) {
			reader.Next();
			data.material = ReaderModelMaterial(reader);
		}
		else if (reader.Equals("children")) {
			reader.Next();

			if (reader.IsArray()) {
				data.childrenCount = reader.Size();
				if (data.childrenCount > 0) {
					data.children = OPALLOC(SceneDataModel, data.childrenCount);
					for (ui32 i = 0; i < data.childrenCount; i++) {
						reader.Next();
						data.children[i] = LoadModel(reader);
					}
				}
			}
		}
		else {
			reader.NextClear();
		}
	}

	return data;
}

OPint SceneLoader(OPstream* str, SceneData** scene) {

	SceneData* result = OPNEW(SceneData);

	TokenReader reader = TokenReader((char*)str->Data, str->Length);

	ui32 s = reader.Size();
	for (ui32 c = 0; c < s; c++) {
		reader.Next();

		OPlog("%s", reader.GetString());

		if (reader.Equals("models")) {
			reader.Next();
			if (reader.IsArray()) {

				ui32 modelsCount = reader.Size();
				result->modelsCount = modelsCount;
				result->models = OPALLOC(SceneDataModel, modelsCount);


				for (ui32 m = 0; m < modelsCount; m++) {
					reader.Next();

					OPlog("%s", reader.GetString());
					result->models[m] = LoadModel(reader);

				}

			}
		}
		else {
			reader.NextClear();
		}
	}

	*scene = result;

	return 1;
}


void UnloadSceneDataMaterial(SceneDataModelMaterial material) {
	if (material.texture != NULL) OPfree(material.texture);
}

void UnloadSceneDataModel(SceneDataModel model) {
	if (model.name != NULL) OPfree(model.name);
	if (model.opm != NULL) OPfree(model.opm);
	if (model.type != NULL) OPfree(model.type);
	if (model.gametype != NULL) OPfree(model.gametype);
	if (model.userData != NULL) OPfree(model.userData);
	UnloadSceneDataMaterial(model.material);
	if (model.children != NULL) {
		for (ui32 j = 0; j < model.childrenCount; j++) {
			UnloadSceneDataModel(model.children[j]);
		}
		OPfree(model.children);
	}
}

OPint SceneUnload(SceneData* scene) {
	
	for (ui32 i = 0; i < scene->modelsCount; i++) {
		SceneDataModel model = scene->models[i];
		UnloadSceneDataModel(model);
	}
	OPfree(scene->models);
	OPfree(scene);

	return 1;
}

OPint SceneReload(OPstream* str, SceneData** script) {

	return 1;
}

void SceneAddLoader() {
	OPassetLoader loaderOPS = {
		".opscene",
		"Scenes/",
		sizeof(SceneData),
		(OPint(*)(OPstream*, void**))SceneLoader,
		(OPint(*)(void*))SceneUnload,
		(OPint(*)(OPstream*, void**))SceneReload
	};
	OPCMAN.AddLoader(&loaderOPS);
}