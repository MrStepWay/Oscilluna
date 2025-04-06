#include "DeviceSelectorComponent.h"

DeviceSelectorComponent::DeviceSelectorComponent(juce::AudioDeviceManager& dm, juce::MidiMessageCollector* collector)
: audioDeviceManager(dm), midiCollector(collector)
{
    jassert(midiCollector != nullptr);
    
    // Подписываемся на изменения в AudioDeviceManager
    audioDeviceManager.addChangeListener(this);
    
    // Заголовок (лейбл)
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, titleColour);
    addAndMakeVisible(titleLabel);

    // MIDI Вход (лейбл)
    midiInputLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    midiInputLabel.setJustificationType(juce::Justification::centredRight);
    midiInputLabel.setColour(juce::Label::textColourId, labelColour);
    midiInputLabel.attachToComponent(&midiInputComboBox, true);
    addAndMakeVisible(midiInputLabel);

    midiInputComboBox.setEditableText(false);
    midiInputComboBox.setJustificationType(juce::Justification::centredLeft);
    midiInputComboBox.setTextWhenNothingSelected("No MIDI Input");
    midiInputComboBox.setTextWhenNoChoicesAvailable("Unavailable");
    // Привязываем лямбда-функцию к событию onChange
    midiInputComboBox.onChange = [this] { midiInputChanged(); };
    addAndMakeVisible(midiInputComboBox);

    // Аудио Выход (лейбл)
    audioOutputLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    audioOutputLabel.setJustificationType(juce::Justification::centredRight);
    audioOutputLabel.setColour(juce::Label::textColourId, labelColour);
    audioOutputLabel.attachToComponent(&audioOutputComboBox, true);
    addAndMakeVisible(audioOutputLabel);

    audioOutputComboBox.setEditableText(false);
    audioOutputComboBox.setJustificationType(juce::Justification::centredLeft);
    audioOutputComboBox.setTextWhenNothingSelected("No Audio Output");
    audioOutputComboBox.setTextWhenNoChoicesAvailable("Unavailable");
    // Привязываем лямбда-функцию к событию onChange
    audioOutputComboBox.onChange = [this] { audioOutputChanged(); };
    addAndMakeVisible(audioOutputComboBox);

    // Кнопка Refresh
    // Привязываем лямбда-функцию к событию onClick кнопки
    refreshButton.onClick = [this] { refreshClicked(); };
    addAndMakeVisible(refreshButton);

    // Начальное заполнение списков
    refreshDeviceLists();
}

DeviceSelectorComponent::~DeviceSelectorComponent() {
    // Отписываемся от изменений.
    audioDeviceManager.removeChangeListener(this);
}

void DeviceSelectorComponent::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, cornerSize);
}

void DeviceSelectorComponent::resized() {
    juce::Rectangle<int> bounds = getLocalBounds();
    
    bounds.reduce(padding, padding);

    // Заголовок
    titleLabel.setBounds(bounds.removeFromTop(20));
    bounds.removeFromTop(padding);
    
    // Кнопка Refresh
    juce::Rectangle<int> buttonArea = bounds.removeFromRight(bounds.getWidth() / 4);
    buttonArea.removeFromTop(buttonArea.getHeight() / 3);
    buttonArea.removeFromBottom(buttonArea.getHeight() / 2);
    buttonArea.removeFromLeft(padding);
    refreshButton.setBounds(buttonArea);
    
    // Строки с выпдающими списками
    juce::Rectangle<int> midiRowArea = bounds.removeFromTop(bounds.getHeight() / 2);
    juce::Rectangle<int> outputRowArea = bounds;
    midiRowArea.removeFromBottom(padding / 2);
    outputRowArea.removeFromTop(padding / 2);
    
    midiRowArea.removeFromLeft(midiRowArea.getWidth() / 3);
    outputRowArea.removeFromLeft(outputRowArea.getWidth() / 3);
    
    midiInputComboBox.setBounds(midiRowArea);
    audioOutputComboBox.setBounds(outputRowArea);
}

/**Функция, вызываемая при нажатии на кнопку Refresh*/
void DeviceSelectorComponent::refreshClicked() {
    refreshDeviceLists();
}

/**Установка нового выбранного Midi-устройства*/
void DeviceSelectorComponent::midiInputChanged() {
    // Получаем текущий выбранный индекс из ComboBox
    int currentSelectedIndex = midiInputComboBox.getSelectedItemIndex();
    juce::Array<juce::MidiDeviceInfo> availableMidiInputs = juce::MidiInput::getAvailableDevices(); // Получаем все входные доступные midi-устройства.

    // Обработка предыдущего устройства, если оно отличается от нового.
    if (lastMidiInputIndex >= 0 && lastMidiInputIndex != currentSelectedIndex) {
        // Получаем список устройств еще раз на всякий случай
        juce::Array<juce::MidiDeviceInfo> previousInputs = juce::MidiInput::getAvailableDevices();
        if (lastMidiInputIndex < previousInputs.size()) {
            juce::MidiDeviceInfo lastDevice = previousInputs[lastMidiInputIndex];
            // Удаляем колбэк со старого устройства
            audioDeviceManager.removeMidiInputDeviceCallback(lastDevice.identifier, midiCollector);
            // Выключаем старое устройство
            audioDeviceManager.setMidiInputDeviceEnabled(lastDevice.identifier, false);
        }
    } else if (lastMidiInputIndex >= 0 && lastMidiInputIndex == currentSelectedIndex) {
         // В этом случае мы не выключаем устройство, но позже переустановим колбэк.
         // Можно на всякий случай удалить колбэк здесь, чтобы гарантировать чистое добавление позже.
         if (currentSelectedIndex >= 0 && currentSelectedIndex < availableMidiInputs.size()) {
             juce::MidiDeviceInfo currentDevice = availableMidiInputs[currentSelectedIndex];
             // Попытка удалить коллбэк, даже если он тот же. Не повредит.
             audioDeviceManager.removeMidiInputDeviceCallback(currentDevice.identifier, midiCollector);
         }
    }


    // Обработка текущего выбранного устройства
    if (currentSelectedIndex >= 0 && currentSelectedIndex < availableMidiInputs.size()) {
        juce::MidiDeviceInfo newDevice = availableMidiInputs[currentSelectedIndex];
        // Проверяем, включено ли устройство уже менеджером
        bool isEnabled = audioDeviceManager.isMidiInputDeviceEnabled(newDevice.identifier);

        // Включаем устройство, если оно еще не включено
        if (!isEnabled) {
            audioDeviceManager.setMidiInputDeviceEnabled(newDevice.identifier, true);
        }

        // Добавляем колбэк (мы его удалили выше, если индекс совпал)
        audioDeviceManager.addMidiInputDeviceCallback(newDevice.identifier, midiCollector);
        // Обновляем индекс последнего успешно настроенного устройства
        lastMidiInputIndex = currentSelectedIndex;
    } else { // Ничего не выбрано
        lastMidiInputIndex = -1;
    }

    // Синхронизация UI (на всякий случай)
    midiInputComboBox.setSelectedItemIndex(lastMidiInputIndex, juce::dontSendNotification); // Используем обновленный lastMidiInputIndex
}

/**Устновка нового устройства вывода аудио**/
void DeviceSelectorComponent::audioOutputChanged() {
    juce::String selectedDeviceName = audioOutputComboBox.getText();

    // Если выбрана опция "No Audio Output" или аналогичное, ничего не делаем
    if (selectedDeviceName.isEmpty() || selectedDeviceName == "No Audio Output" || selectedDeviceName == "Unavailable") {
        return;
    }

    // Получаем текущие настройки
    juce::AudioDeviceManager::AudioDeviceSetup currentSetup;
    audioDeviceManager.getAudioDeviceSetup(currentSetup);

    // Если выбранное имя совпадает с текущим, ничего не делаем
    if (currentSetup.outputDeviceName == selectedDeviceName)
        return;

    // Создаем новые настройки, копируя текущие
    juce::AudioDeviceManager::AudioDeviceSetup newSetup = currentSetup;
    // Устанавливаем новое имя выходного устройства
    newSetup.outputDeviceName = selectedDeviceName;

    // Пытаемся применить новые настройки
    juce::String error = audioDeviceManager.setAudioDeviceSetup(newSetup, true); // true = treatAsChosenDevice

    // Если есть ошибка, то уведомляем пользователя.
    if (error.isNotEmpty()) {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                               "Audio Output Error",
                                               "Could not set audio output device:\n" + error);
        // Обновляем список, чтобы ComboBox показал реальное активное устройство, так как попытка установки не удалась.
        refreshDeviceLists();
    }
    // Если ошибки нет, настройки применены. ComboBox уже показывает выбранное значение.
}

/**Обновление списка midi-устройств**/
void DeviceSelectorComponent::populateMidiInputList() {
    midiInputComboBox.clear(juce::dontSendNotification);
    juce::Array<juce::MidiDeviceInfo> midiInputs = juce::MidiInput::getAvailableDevices();
    juce::StringArray midiInputNames;
    int currentDeviceIndex = -1;
    juce::String currentDeviceIdentifier;

    // Ставим первое устройство, как устройство по-умолчанию при обновлении списка.
    for (const juce::MidiDeviceInfo& device : midiInputs) {
        if (audioDeviceManager.isMidiInputDeviceEnabled(device.identifier)) {
            currentDeviceIdentifier = device.identifier;
            break;
        }
    }

    // По идентификатору получаем индекс выбранного устройства.
    for (int i = 0; i < midiInputs.size(); ++i) {
        midiInputNames.add(midiInputs[i].name);
        if (midiInputs[i].identifier == currentDeviceIdentifier) {
            currentDeviceIndex = i;
        }
    }

    midiInputComboBox.addItemList(midiInputNames, 1);

    // Выбираем активное (или первое доступное) устройство в ComboBox.
    if (currentDeviceIndex != -1) {
        lastMidiInputIndex = currentDeviceIndex;
        midiInputComboBox.setSelectedItemIndex(currentDeviceIndex, juce::dontSendNotification);
    } else if (midiInputs.size() > 0) {
         lastMidiInputIndex = 0;
         midiInputComboBox.setSelectedItemIndex(0, juce::dontSendNotification);
    } else {
        lastMidiInputIndex = -1;
        midiInputComboBox.setTextWhenNoChoicesAvailable("No MIDI devices");
    }
}

/**Обновление списка доступныз выходных аудиоi-устройств**/
void DeviceSelectorComponent::populateAudioOutputList() {
    audioOutputComboBox.clear(juce::dontSendNotification); // Чистим выпадающий список.
    
    // Копируем настройки AudioDeviceSetup
    juce::AudioDeviceManager::AudioDeviceSetup currentSetup;
    audioDeviceManager.getAudioDeviceSetup(currentSetup);

    // Получаем тип устройства, который сейчас активен в AudioDeviceManager
    juce::AudioIODeviceType* currentDeviceType = audioDeviceManager.getCurrentDeviceTypeObject();
    if (currentDeviceType == nullptr) {
        // Если тип устройства не установлен, список будет пустым
        audioOutputComboBox.setTextWhenNoChoicesAvailable("No device type selected");
        return;
    }

    // Получаем имена устройств для текущего активного типа
    juce::StringArray deviceNames = currentDeviceType->getDeviceNames();

    int itemIdCounter = 1;
    int selectedItemId = -1;

    for (const juce::String& name : deviceNames) {
        audioOutputComboBox.addItem(name, itemIdCounter);
        // Отмечаем элемент как выбранный, если имя совпадает с текущим
        if (name == currentSetup.outputDeviceName) {
            selectedItemId = itemIdCounter;
        }
        itemIdCounter++;
    }

     if (selectedItemId != -1) {
        audioOutputComboBox.setSelectedId(selectedItemId, juce::dontSendNotification);
     } else if (audioOutputComboBox.getNumItems() > 0) {
         // Если текущее устройство вывода не найдено в списке, то выбираем первый элемент из списка ТЕКУЩЕГО типа.
         audioOutputComboBox.setSelectedItemIndex(0, juce::dontSendNotification);
     } else {
         // Если для текущего типа драйвера нет устройств
         audioOutputComboBox.setTextWhenNoChoicesAvailable("No devices for this type");
     }
}

/**Обновление списков с аудио и midi устройствами**/
void DeviceSelectorComponent::refreshDeviceLists() {
    populateMidiInputList();
    populateAudioOutputList();
}

/**Слушатель изменений в AudioDeviceManager. В случае изменений обновляем списки и выбираем устройство по умолчанию**/
void DeviceSelectorComponent::changeListenerCallback(juce::ChangeBroadcaster* source) {
    // Проверяем, что сообщение пришло именно от нашего AudioDeviceManager
    if (source == &audioDeviceManager) {
        refreshDeviceLists();
        midiInputChanged();
    }
}
