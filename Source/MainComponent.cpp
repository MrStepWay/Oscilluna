#include "MainComponent.h"


MainComponent::MainComponent() : synthAudioSource(keyboardState), keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
envelopeComponent([this](float v) { forEachVoice([&](SynthVoice& sv){ sv.setAttack(v); }); }, // Attack
                  [this](float v) { forEachVoice([&](SynthVoice& sv){ sv.setDecay(v); }); }, // Decay
                  [this](float v) { forEachVoice([&](SynthVoice& sv){ sv.setSustain(v); }); }, // Sustain
                  [this](float v) { forEachVoice([&](SynthVoice& sv){ sv.setRelease (v); }); }), // Release
settingsComponent(deviceManager,
                  synthAudioSource.getMidiCollector(),
                  [this](bool isRec){ handleRecordingStateChange(isRec); },
                  [this](){ savePreset(); },
                  [this](){ loadPreset(); })
{
    setLookAndFeel(&lf);
    
    addAndMakeVisible(oscParamsComponent1);
    addAndMakeVisible(oscParamsComponent2);
    addAndMakeVisible(oscParamsComponent3);
    addAndMakeVisible(keyboardComponent);
    
    // Некоторые платформы требуют разрешения на открытие входных каналов, поэтому запрашиваем его здесь.
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio)) {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    } else {
        setAudioChannels(0, 2);
    }
    
    // Устанавливаем колбеки для 1-ого осциллятора.
    oscParamsComponent1.onVolume = [this](float v){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(0).setOscLevel(v); }); };
    oscParamsComponent1.onPitch = [this](float p){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(0).setPitchSemi((int)p); }); };
    oscParamsComponent1.onVoices = [this](int n){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(0).setVoices(n); }); };
    oscParamsComponent1.onDetune = [this](float d){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(0).setDetune(d); }); };
    oscParamsComponent1.onMain = [this](float m){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(0).setMainLevel(m); }); };
    oscParamsComponent1.onSide = [this](float s){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(0).setSideLevel(s); }); };
    oscParamsComponent1.onShape = [this](int i){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(0).setMode(static_cast<Oscillator::OscillatorMode>(i)); }); };

    // Устанавливаем колбеки для 2-ого осциллятора.
    oscParamsComponent2.onVolume = [this](float v){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(1).setOscLevel(v); }); };
    oscParamsComponent2.onPitch = [this](float p){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(1).setPitchSemi((int)p); }); };
    oscParamsComponent2.onVoices = [this](int n){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(1).setVoices(n); }); };
    oscParamsComponent2.onDetune = [this](float d){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(1).setDetune(d); }); };
    oscParamsComponent2.onMain = [this](float m){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(1).setMainLevel(m); }); };
    oscParamsComponent2.onSide = [this](float s){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(1).setSideLevel(s); }); };
    oscParamsComponent2.onShape = [this](int i){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(1).setMode(static_cast<Oscillator::OscillatorMode>(i)); }); };
    
    // Устанавливаем колбеки для 3-его осциллятора.
    oscParamsComponent3.onVolume = [this](float v){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(2).setOscLevel(v); }); };
    oscParamsComponent3.onPitch = [this](float p){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(2).setPitchSemi((int)p); }); };
    oscParamsComponent3.onVoices = [this](int n){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(2).setVoices(n); }); };
    oscParamsComponent3.onDetune = [this](float d){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(2).setDetune(d); }); };
    oscParamsComponent3.onMain = [this](float m){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(2).setMainLevel(m); }); };
    oscParamsComponent3.onSide = [this](float s){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(2).setSideLevel(s); }); };
    oscParamsComponent3.onShape = [this](int i){ forEachVoice([&](SynthVoice& sv){ sv.getOsc(2).setMode(static_cast<Oscillator::OscillatorMode>(i)); }); };
    
    // Envelope
    addAndMakeVisible(envelopeComponent);
    
    // Settings
    addAndMakeVisible(settingsComponent);
    
    setSize(1000, 600);
}

MainComponent::~MainComponent() {
    // Убедимся, что запись остановлена и файл закрыт при выходе
    if (isRecordingToFile.load()) {
        handleRecordingStateChange(false); // Останавливаем запись
    }
    
    setLookAndFeel(nullptr);
    shutdownAudio();
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate) {
    synthAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) {
    // Сначала генерируем звук синтезатора
    synthAudioSource.getNextAudioBlock(bufferToFill);

    // Затем передаем сгенерированный звук на запись (если активна)
    processAudioForRecording(bufferToFill);
}

void MainComponent::releaseResources() {
    synthAudioSource.releaseResources(); // Освобождаем ресурсы источника аудио (как следствие и синтезатора)
    // Останавливаем запись, если она была активна
    if (isRecordingToFile.load()) {
        handleRecordingStateChange(false);
    }
}

void MainComponent::paint (juce::Graphics& g) {
    // Заполняем окно цветом
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized() {
    int padding = 10;
    juce::Rectangle<int> area = getLocalBounds().reduced(padding);
    
    juce::Rectangle<int> keyboardArea = area.removeFromBottom(80);
    keyboardArea.removeFromTop(padding / 2);
    keyboardComponent.setBounds(keyboardArea);
    
    juce::Rectangle<int> rightArea = area.removeFromRight(area.getWidth() / 3);
    area.removeFromRight(padding / 2);
    rightArea.removeFromLeft(padding / 2);
    
    oscParamsComponent1.setBounds(area.removeFromTop(area.getHeight() / 3).reduced(0, padding / 2));
    oscParamsComponent2.setBounds(area.removeFromTop(area.getHeight() / 2).reduced(0, padding / 2));
    oscParamsComponent3.setBounds(area.removeFromTop(area.getHeight()).reduced(0, padding / 2));
    
    juce::Rectangle<int> envelopeArea = rightArea.removeFromBottom(rightArea.getHeight() / 2).reduced(0, padding / 2);
    envelopeComponent.setBounds(envelopeArea);
    settingsComponent.setBounds(rightArea.reduced(0, padding / 2));
}

void MainComponent::forEachVoice(std::function<void(SynthVoice&)> fn) {
    auto& sy = synthAudioSource.getSynth();
    for (int i = 0; i < sy.getNumVoices(); ++i) {
        if (auto* sv = dynamic_cast<SynthVoice*>(sy.getVoice(i))) {
            fn(*sv);
        }
    }
}

// Запись аудио
void MainComponent::handleRecordingStateChange(bool isNowRecording) {
    if (isNowRecording) {
        if (isRecordingToFile.load()) { // Если почему-то уже идёт запись, то сначала её останавливаем
            handleRecordingStateChange(false);
        }

        // Создаем временный файл WAV
        tempAudioFile = std::make_unique<juce::TemporaryFile>(".wav");
        juce::File tempFile = tempAudioFile->getFile();

        // Создаем поток для временного файла
        auto fileStream = tempFile.createOutputStream();
        if (fileStream == nullptr) {
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Recording Error", "Could not create temporary file stream!");
            settingsComponent.resetRecordingStateVisuals();
            tempAudioFile.reset();
            return;
        }

        // Создаем писатель WAV
        juce::WavAudioFormat wavFormat;
        auto* device = deviceManager.getCurrentAudioDevice();
        if (!device) {
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Recording Error", "No audio device available!");
            settingsComponent.resetRecordingStateVisuals();
            tempAudioFile.reset();
            return;
        }
        
        double currentSampleRate = device->getCurrentSampleRate();
        unsigned int numChannels = device->getActiveOutputChannels().countNumberOfSetBits();
        if (numChannels == 0) {
            numChannels = 2;
        }

        // Блокируем доступ к audioWriter
        {
            const juce::ScopedLock sl(writerLock);
            audioWriter.reset(wavFormat.createWriterFor(fileStream.release(),
                                                         currentSampleRate,
                                                         numChannels,
                                                         32, // Битность
                                                         {}, 0));
        }

        if (audioWriter == nullptr) {
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Recording Error", "Could not create WAV writer for temporary file!");
            settingsComponent.resetRecordingStateVisuals();
            tempAudioFile.reset();
            return;
        }

        // Устанавливаем флаг записи
        isRecordingToFile.store(true);
        
    } else {
        settingsComponent.resetRecordingStateVisuals(); // Обновляем UI на всякий случай
        if (isRecordingToFile.load()) {
            isRecordingToFile.store(false); // Останавливаем запись в аудиопотоке

            juce::File tempWavFile; // Сохраним путь к временному файлу

            // Блокировка для финализации записи
            {
                const juce::ScopedLock sl(writerLock);
                if (audioWriter != nullptr) {
                    if (tempAudioFile != nullptr) {
                        tempWavFile = tempAudioFile->getFile();
                    }
                    audioWriter.reset(); // Закрываем временный WAV
                } else {
                     tempAudioFile.reset();
                     return; // Нечего сохранять
                }
            }

            // Проверяем, успешно ли был создан временный файл
            if (!tempWavFile.existsAsFile()) {
                 tempAudioFile.reset();
                 return;
            }

            // Показываем диалог сохранения файла
            juce::FileChooser fileChooser ("Save Recorded Audio...",
                                           juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
                                           "*.wav",
                                           true);

            if (fileChooser.browseForFileToSave(true)) {
                juce::File chosenFile = fileChooser.getResult();

                if (chosenFile.getFileExtension().toLowerCase() != ".wav") {
                     chosenFile = chosenFile.withFileExtension(".wav");
                }

                // Перемещаем временный WAV файл
                if (!tempWavFile.moveFileTo(chosenFile)) { // Если не получилось с первого раза, то пробуем не переместить, а скопировать
                    if (tempWavFile.copyFileTo(chosenFile)) {
                         tempWavFile.deleteFile(); // Удаляем оригинал после копии
                    } else { // Если и так не получилось, то уведомляем пользователя об ошибке
                         juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                                                "Save Error", "Could not save recorded file to:\n" + chosenFile.getFullPathName());
                         tempWavFile.deleteFile(); // Удаляем временный в любом случае
                    }
                }
            } else { // Пользователь отменил диалог сохранения
                tempWavFile.deleteFile(); // Удаляем временный файл
            }
            tempAudioFile.reset();

        }
    }
}

void MainComponent::processAudioForRecording(const juce::AudioSourceChannelInfo& bufferToFill) {
    // Проверяем, активна ли запись
    if (isRecordingToFile.load()) {
        const juce::ScopedLock sl(writerLock); // Блокируем доступ к audioWriter
        if (audioWriter != nullptr) {
            // Записываем данные из буфера bufferToFill.buffer
            audioWriter->writeFromAudioSampleBuffer(*bufferToFill.buffer,
                                                     bufferToFill.startSample,
                                                     bufferToFill.numSamples);
        }
    }
}


void MainComponent::savePreset() {
    // Получаем или создаём папку с пресетами.
    juce::File presetFolder = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("Oscilluna Presets");
    if (!presetFolder.exists()) {
            presetFolder.createDirectory();
    }
    
    juce::FileChooser fileChooser("Save Preset As...",
                                  juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("Oscilluna Presets"),
                                  "*.json");

    if (fileChooser.browseForFileToSave(true)) { // Если пользователь не отказался от сохранения
        juce::File presetFile = fileChooser.getResult();
        // Убедимся, что расширение .json
        if (!presetFile.hasFileExtension("json")) {
            presetFile = presetFile.withFileExtension("json");
        }

        juce::var presetData = getCurrentPresetData(); // Собираем настройки осцилляторов и Envelope

        if (!presetData.isObject()) {
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Save Error", "Could not read current synth state.");
            return;
        }

        // Добавляем имя пресета в данные
        if (auto* obj = presetData.getDynamicObject()) {
            obj->setProperty("presetName", presetFile.getFileNameWithoutExtension());
        }

        juce::String jsonString = juce::JSON::toString(presetData, true);

        // Пытаемся сохранить данные в файл
        if (presetFile.replaceWithText(jsonString)) {
            settingsComponent.setPresetName(presetFile.getFileNameWithoutExtension());
        } else { // При неудаче уведомляем пользователя.
             juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Save Error", "Could not write to file.");
        }
    }
}

void MainComponent::loadPreset() {
    juce::File presetFolder = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("Oscilluna Presets");
    presetFolder.createDirectory(); // Создаем папку, если ее нет (на всякий случай)
    
    juce::FileChooser fileChooser("Load Preset...",
                                  juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("Oscilluna Presets"),
                                  "*.json");

    if (fileChooser.browseForFileToOpen()) { // Если пользователь не отменил загрузку
        // Открываем выбранный файл и парсим данные
        juce::File presetFile = fileChooser.getResult();
        juce::String jsonString = presetFile.loadFileAsString();
        juce::var presetData;
        juce::Result parseResult = juce::JSON::parse(jsonString, presetData);

        if (parseResult.wasOk()) { // Если парсинг успешен
            applyPresetData(presetData); // Применяем данные
            // Обновляем имя в UI
            if (presetData.hasProperty("presetName")) {
                settingsComponent.setPresetName(presetData["presetName"].toString());
            } else {
                settingsComponent.setPresetName(presetFile.getFileNameWithoutExtension());
            }
        } else { // Если произошла ошибка во время парсинга, то уведомляем пользователя
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Load Error",
                                                   "Could not parse preset file.\nError: " + parseResult.getErrorMessage());
            settingsComponent.setPresetName("Load Error");
        }
    }
}

juce::var MainComponent::getCurrentPresetData() {
    // Собираем данные из первого голоса (все голоса одинаковые)
    auto& synth = synthAudioSource.getSynth();
    
    if (synth.getNumVoices() == 0) { // Если голосов нет, то возвращаем пустоту
        return {};
    }

    SynthVoice* representativeVoice = dynamic_cast<SynthVoice*>(synth.getVoice(0)); // Голос-представитель
    if (!representativeVoice) {
        return {};
    }

    auto* presetObject = new juce::DynamicObject();
    presetObject->setProperty("presetFormatVersion", 1); // Версия

    // Собираем данные с осцилляторов
    juce::Array<juce::var> oscillatorsArray;
    for (int i = 0; i < 3; ++i) {
        auto& osc = representativeVoice->getOsc(i);
        auto* oscData = new juce::DynamicObject();
        oscData->setProperty("volume", osc.getOscLevel());
        oscData->setProperty("pitch", osc.getPitchSemi());
        oscData->setProperty("shape", static_cast<int>(osc.getMode()));

        auto* unisonData = new juce::DynamicObject();
        unisonData->setProperty("voices", osc.getVoices());
        unisonData->setProperty("detune", osc.getDetune());
        unisonData->setProperty("mainVol", osc.getMainLevel());
        unisonData->setProperty("sideVol", osc.getSideLevel());
        oscData->setProperty("unison", juce::var(unisonData));

        oscillatorsArray.add(juce::var(oscData));
    }
    presetObject->setProperty("oscillators", oscillatorsArray);

    // Собираем данные с envelope
    auto* envelopeData = new juce::DynamicObject();
    auto& adsrParams = representativeVoice->getAdsrParams();
    envelopeData->setProperty("attack", adsrParams.attack);
    envelopeData->setProperty("decay", adsrParams.decay);
    envelopeData->setProperty("sustain", adsrParams.sustain);
    envelopeData->setProperty("release", adsrParams.release);
    presetObject->setProperty("envelope", juce::var(envelopeData));

    return juce::var(presetObject);
}

// Применяет загруженные данные
void MainComponent::applyPresetData(const juce::var& data) {
    if (!data.isObject()) { // Если что-то не так, то ничего не делаем
        return;
    }

    // Применение envelope
    juce::ADSR::Parameters newAdsrParams;
    if (data.hasProperty("envelope") && data["envelope"].isObject()) { // Если настройки для Envelope вообще есть
        auto envelopeData = data["envelope"];
        newAdsrParams.attack = getFloatVarOrDefault(envelopeData, "attack", 0.01f);
        newAdsrParams.decay = getFloatVarOrDefault(envelopeData, "decay", 0.1f);
        newAdsrParams.sustain = getFloatVarOrDefault(envelopeData, "sustain", 1.0f);
        newAdsrParams.release = getFloatVarOrDefault(envelopeData, "release", 0.5f);
    }

    // Применяем ко всем голосам
    forEachVoice([&](SynthVoice& sv) {
        sv.setAttack(newAdsrParams.attack);
        sv.setDecay(newAdsrParams.decay);
        sv.setSustain(newAdsrParams.sustain);
        sv.setRelease(newAdsrParams.release);
    });

    // Обновляем UI envelope
    envelopeComponent.setAttackSliderValue(newAdsrParams.attack, juce::dontSendNotification);
    envelopeComponent.setDecaySliderValue(newAdsrParams.decay, juce::dontSendNotification);
    envelopeComponent.setSustainSliderValue(newAdsrParams.sustain, juce::dontSendNotification);
    envelopeComponent.setReleaseSliderValue(newAdsrParams.release, juce::dontSendNotification);

    // Применяем осцилляторы
    if (data.hasProperty("oscillators") && data["oscillators"].isArray()) { // Если есть информация по осцилляторам
        auto& oscillatorsArray = data["oscillators"];
        // Используем массив компонентов осцилляторов
        std::array<OscParamsComponent*, 3> oscParamComponentsPtrs = {&oscParamsComponent1, &oscParamsComponent2, &oscParamsComponent3};

        for (int i = 0; i < 3 && i < oscillatorsArray.size(); ++i) {
            if (!oscillatorsArray[i].isObject()) { // Если что-то не так, то ничего не делаем и продолжаем
                continue;
            }
            auto oscData = oscillatorsArray[i];

            // Собираем данные из файла
            float volume = getFloatVarOrDefault(oscData, "volume", 0.5f);
            int pitch = getIntVarOrDefault(oscData, "pitch", 0);
            int shapeInt = getIntVarOrDefault(oscData, "shape", (int)Oscillator::OscillatorMode::Sine);
            auto shape = static_cast<Oscillator::OscillatorMode>(juce::jlimit(0, (int)Oscillator::NumOscillatorModes - 1, shapeInt));

            int unisonVoices = 1;
            float unisonDetune = 0.0f;
            float unisonMainVol = 1.0f;
            float unisonSideVol = 0.5f;
            if (oscData.hasProperty("unison") && oscData["unison"].isObject()) { // Если есть unison
                 auto unisonData = oscData["unison"];
                 unisonVoices = getIntVarOrDefault(unisonData, "voices", 1);
                 unisonDetune = getFloatVarOrDefault(unisonData, "detune", 0.0f);
                 unisonMainVol = getFloatVarOrDefault(unisonData, "mainVol", 1.0f);
                 unisonSideVol = getFloatVarOrDefault(unisonData, "sideVol", 0.5f);
            }

            // Применяем ко всем голосам
            forEachVoice([&](SynthVoice& sv) {
                 auto& osc = sv.getOsc(i);
                 osc.setOscLevel(volume);
                 osc.setPitchSemi(pitch);
                 osc.setMode(shape);
                 osc.setVoices(unisonVoices);
                 osc.setDetune(unisonDetune);
                 osc.setMainLevel(unisonMainVol);
                 osc.setSideLevel(unisonSideVol);
            });

            // Обновляем UI Осциллятора
            if (oscParamComponentsPtrs[i]) { // Проверка на nullptr
               oscParamComponentsPtrs[i]->setVolumeValue(volume, juce::dontSendNotification);
               oscParamComponentsPtrs[i]->setPitchValue(pitch, juce::dontSendNotification);
               oscParamComponentsPtrs[i]->setShapeIndex(static_cast<int>(shape), juce::dontSendNotification);
               oscParamComponentsPtrs[i]->setUnisonVoicesValue(unisonVoices, juce::dontSendNotification);
               oscParamComponentsPtrs[i]->setUnisonDetuneValue(unisonDetune, juce::dontSendNotification);
               oscParamComponentsPtrs[i]->setUnisonMainVolValue(unisonMainVol, juce::dontSendNotification);
               oscParamComponentsPtrs[i]->setUnisonSideVolValue(unisonSideVol, juce::dontSendNotification);
            }
        }
    }
}

// Хелперы для валидации
float MainComponent::getFloatVarOrDefault(const juce::var& parent, const juce::Identifier& key, float defaultValue) {
    if (parent.hasProperty(key)) { // Если есть такой ключ и значение, то возвращаем его
        const auto& val = parent[key];
        if (val.isDouble()) {
            return (float) static_cast<double>(val);
        }
        if (val.isInt()) {
            return (float) static_cast<int>(val);
        }
        if (val.isInt64()) {
            return (float) static_cast<juce::int64>(val);
        }
    }
    return defaultValue; // Иначе возвращаем дефолтное
}

int MainComponent::getIntVarOrDefault(const juce::var& parent, const juce::Identifier& key, int defaultValue) {
     if (parent.hasProperty(key)) { // Если есть такой ключ и значение, то возвращаем его
        const auto& val = parent[key];
         if (val.isInt()) {
             return static_cast<int>(val);
         }
         if (val.isInt64()) {
             return static_cast<int>(static_cast<juce::int64>(val));
         }
         if (val.isDouble()) {
             return static_cast<int>(std::round(static_cast<double>(val))); // Округляем double
         }
     }
     return defaultValue; // Иначе возвращаем дефолтное
}
