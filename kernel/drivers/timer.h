/*  
    Copyright (C) 2026 Luna Moonlit Noir <lunaNoir.sk@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdint.h>

#ifndef TIMER_H
#define TIMER_H

void timer_init(uint32_t frequency);
void timer_handler();
uint64_t get_uptime();
void sleep(uint64_t ms);
void safe_sleep(uint32_t ms);

#endif