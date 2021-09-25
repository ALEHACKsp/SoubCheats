#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <tchar.h>
#include <stdio.h>
#include <sstream>
#include <ctime>
#include <string>
#include <iostream>
#include <conio.h>

#include "../Dependencies/ImGui/imgui.h"
#include "../Dependencies/ImGui/imgui_impl_dx9.h"
#include "../Dependencies/ImGui/imgui_impl_win32.h"
#include "../Dependencies/ImGui/imgui_internal.h"

#include "../SDK/CGlobalVarsBase.h"
#include "../SDK/ICvar.h"

char InputBuf[256];
ImVector<char*> Items;
ImVector<const char*> Commands;
ImVector<char*> History;
int HistoryPos;
ImGuiTextFilter Filter;
bool AutoScroll;
bool ScrollToBottom;

void ClearLogs()
{
    for (int i = 0; i < Items.Size; i++)
        free(Items[i]);
    Items.clear();
}

static void TrimStr(char* str)
{
    char* str_end = str + strlen(str);
    while (str_end > str && str_end[-1] == ' ')
        str_end--; *str_end = 0;
}

void AddLog(const char* fmt, ...) IM_FMTARGS(2)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);
    Items.push_back(_strdup(buf));
}

void InitConsole()
{
    ClearLogs();
    memset(InputBuf, 0, sizeof(InputBuf));
    HistoryPos = -1;
    Commands.push_back("help");
    Commands.push_back("clear");
    AutoScroll = true;
    ScrollToBottom = false;
}

void ExecuteCommand(const char* CommandLine)
{
    AddLog("%s\n", CommandLine);

    HistoryPos = -1;
    for (int i = History.Size - 1; i >= 0; i--) {
        if (!strcmp(History[i], CommandLine))
        {
            free(History[i]);
            History.erase(History.begin() + i);
            break;
        }
    }
    History.push_back(_strdup(CommandLine));

    if (!strcmp(CommandLine, "help"))
    {
        AddLog("avaliable commands:");
        AddLog("- help");
        AddLog("- clear");
    }
    else if (!strcmp(CommandLine, "clear"))
    {

    }
    else
    {
        AddLog("[error] the command '%s' does not exist.\n", CommandLine);
    }

    ScrollToBottom = true;
}

int TextEditCallback(ImGuiInputTextCallbackData* data)
{
    switch (data->EventFlag)
    {
    case ImGuiInputTextFlags_CallbackCompletion:
    {
        const char* word_end = data->Buf + data->CursorPos;
        const char* word_start = word_end;
        while (word_start > data->Buf)
        {
            const char c = word_start[-1];
            if (c == ' ' || c == '\t' || c == ',' || c == ';')
                break;
            word_start--;
        }

        ImVector<const char*> candidates;
        for (int i = 0; i < Commands.Size; i++)
            if (!_strnicmp(Commands[i], word_start, (int)(word_end - word_start)))
                candidates.push_back(Commands[i]);

        if (candidates.Size == 0)
        {
            AddLog("no match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
        }
        else if (candidates.Size == 1)
        {
            data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
            data->InsertChars(data->CursorPos, candidates[0]);
            data->InsertChars(data->CursorPos, " ");
        }
        else
        {
            int match_len = (int)(word_end - word_start);
            for (;;)
            {
                int c = 0;
                bool all_candidates_matches = true;
                for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                    if (i == 0)
                        c = toupper(candidates[i][match_len]);
                    else if (c == 0 || c != toupper(candidates[i][match_len]))
                        all_candidates_matches = false;
                if (!all_candidates_matches)
                    break;
                match_len++;
            }

            if (match_len > 0)
            {
                data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
            }

            AddLog("possible matches:\n");
            for (int i = 0; i < candidates.Size; i++)
                AddLog("- %s\n", candidates[i]);
        }

        break;
    }
    case ImGuiInputTextFlags_CallbackHistory:
    {
        const int prev_history_pos = HistoryPos;
        if (data->EventKey == ImGuiKey_UpArrow)
        {
            if (HistoryPos == -1)
                HistoryPos = History.Size - 1;
            else if (HistoryPos > 0)
                HistoryPos--;
        }
        else if (data->EventKey == ImGuiKey_DownArrow)
        {
            if (HistoryPos != -1)
                if (++HistoryPos >= History.Size)
                    HistoryPos = -1;
        }

        if (prev_history_pos != HistoryPos)
        {
            const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, history_str);
        }
    }
    }
    return 0;
}

static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
    return TextEditCallback(data);
}

void RenderConsole()
{
    char buf[128] = { 0 };

    ImGui::BeginChild("##console", { 0, 315 }, true);

    ImGui::TextColored(ImColor(255, 255, 0, 255), "Type 'help' for a list of commands");

    for (int i = 0; i < Items.Size; i++) {
        const char* item = Items[i];
        if (!Filter.PassFilter(item))
            continue;

        ImGui::TextUnformatted(item);
    }

    ImGui::EndChild();

    ImGui::SetNextItemWidth(435);
    bool reclaim_focus = false;
    if (ImGui::InputText("##Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub))
    {
        char* s = InputBuf;
        TrimStr(s);
        if (s[0])
            ExecuteCommand(s);
        strcpy(s, "");
        reclaim_focus = true;
    }

    ImGui::SetCursorPos({ 447, 370 });

    if (ImGui::Button("Enter", { 45, 19 })) {
        char* s = InputBuf;
        TrimStr(s);
        if (s[0])
            ExecuteCommand(s);
        strcpy(s, "");
        reclaim_focus = true;
    }
}