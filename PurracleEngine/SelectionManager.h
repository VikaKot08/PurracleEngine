#pragma once
#include <functional>
#include <vector>

class Model;

class SelectionManager
{
public:
    SelectionManager() : selectedModel(nullptr) {}

    void Select(Model* model);
    Model* GetSelected() const { return selectedModel; }

    void AddListener(std::function<void(Model*)> callback);
    void ClearSelection();

private:
    Model* selectedModel;
    std::vector<std::function<void(Model*)>> listeners;

    void NotifyListeners();
};

