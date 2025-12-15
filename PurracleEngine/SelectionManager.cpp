#include "SelectionManager.h"
#include "Model.h"

void SelectionManager::Select(Model* model)
{
    if (selectedModel != model)
    {
        selectedModel = model;
        NotifyListeners();
    }
}

void SelectionManager::AddListener(std::function<void(Model*)> callback)
{
    listeners.push_back(callback);
}

void SelectionManager::ClearSelection()
{
    Select(nullptr);
}

void SelectionManager::NotifyListeners()
{
    for (auto& listener : listeners)
    {
        listener(selectedModel);
    }
}