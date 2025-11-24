#pragma once
#include "MessageQueue.h"
#include <string>
#include <vector>
#include <unordered_map>

class Mesh;
class MessageQueue;

struct MeshData
{
	std::string path;
	std::vector<Mesh*> meshes;
	int refCount = 0;
};

class MeshManager : public MessageQueue
{
public:
	MeshManager();
	~MeshManager();

	static void Allocate();
	static void Deallocate();
	static MeshManager* Get();

	std::vector<Mesh*>* LoadMeshesAssimp(const std::string& path);
	std::vector<Mesh*>* LoadMeshes(const std::string& path);
	void ReleaseMeshes(const std::string& path);
	void ClearCache();

	void SetReplyQueue(MessageQueue* replyQueue);

protected:
	void ProcessMessage(Message* aMessage) override;

private:
	static MeshManager* instance;
	std::unordered_map<std::string, MeshData> meshCache;
	MessageQueue* replyQueue;
	int nextRequestId;
};
