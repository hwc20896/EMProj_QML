#pragma once

namespace EMProj_QML_Backend{
    namespace Constants::Paths {
        //  Sound Effects

        //  BINGO sound effect created by Betty. Sounds funny.
        inline constexpr auto SND_BINGO_PATH = "qrc:/effects/sounds/bingo.wav";
        //  OHNO sound effect also created by Betty.
        inline constexpr auto SND_OHNO_PATH = "qrc:/effects/sounds/ohno.wav";
        //  Sound effect played in Deltarune. Same purpose as the BINGO sound effect.
        inline constexpr auto SND_WON_PATH = "qrc:/effects/sounds/snd_won.wav";
        //  Sound effect played in Deltarune which has the same purpose as OHNO sound effect.
        inline constexpr auto SND_ERROR_PATH = "qrc:/effects/sounds/snd_error.wav";

        //  Sound effect when a wrong option is being SLASHed.
        inline constexpr auto SND_SLASH_PATH = "qrc:/effects/sounds/snd_knight_cut2.wav";
        //  Sound when the RULE page opens.
        inline constexpr auto SND_RULE_OPEN_PATH = "qrc:/effects/sounds/snd_board_text_main.wav";
        //  Sound when the RULE page closes.
        inline constexpr auto SND_RULE_CLOSE_PATH = "qrc:/effects/sounds/snd_board_text_main_end.wav";
        //  Sound when an ability is available.
        inline constexpr auto SND_ABILITY_AVAILABLE_PATH = "qrc:/effects/sounds/snd_boost.wav";
        //  Sound when Mute Switch status is being altered.
        inline constexpr auto SND_STATUS_ALTER = "qrc:/effects/sounds/snd_click.wav";
        //  Sound when the ability isn't yet unlocked.
        inline constexpr auto SND_NOT_UNLOCKED = "qrc:/effects/sounds/snd_cantselect.wav";
        //  Sound when activating the Revive ability.
        inline constexpr auto SND_REVIVE_ACTIVATING = "qrc:/effects/sounds/snd_ominous_cancel.wav";
        //  Sound when deactivating the Revive ability.
        inline constexpr auto SND_REVIVE_DEACTIVATING = "qrc:/effects/sounds/snd_ominous.wav";
        //  Background Music

        //  Created by OMFG.
        inline constexpr auto BGM_PIZZA = "qrc:/music/sounds/mus_pizza.mp3";
        //  Being used in Lambdarune: Maximike fight.
        inline constexpr auto BGM_MAXIMIKE = "qrc:/music/sounds/mus_showtime_is_over.mp3";
        //  Used in Deltarune Ch. 2: Castle Town.
        inline constexpr auto BGM_STATUS_CHECK = "qrc:/music/sounds/mus_my_castle_town.mp3";

        //  IO/Database files

        //  File where Questions are being stored.
        inline constexpr auto DB_QUESTION_FILE = "question_data.xlsx";
        //  File where ranks are being stored.
        inline constexpr auto DB_PODIUM_FILE = "podium_data.db";
    }

    namespace Constants::Literals {
        //  Controllers
        inline constexpr bool USE_OLD_SOUND_EFFECT = false;
        inline constexpr bool USE_OLD_BGM = false;
    }

    namespace Paths = Constants::Paths;
    namespace Literals = Constants::Literals;
}