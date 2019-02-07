-- Copyright 2018 Delft University of Technology
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.math_real.all;

library work;
use work.Encoding.all;
-- Fletcher utils, for use of the log2ceil function
use work.Utils.all;

-- Todo: Description

entity VarIntDecoder is
  generic (
    -- Number of bits in output integer
    INT_BIT_WIDTH               : natural;

    -- Is the varint also zigzag encoded
    ZIGZAG_ENCODED              : boolean
  );
  port (
    -- Rising-edge sensitive clock
    clk                         : in std_logic;

    -- Active high reset
    reset                       : in std_logic;

    -- Signal indicating next byte will be start of a varint that needs to be decoded
    start                       : in std_logic;

    -- Input data (one byte of the varint per clock cycle)
    in_data                     : in std_logic_vector(7 downto 0);

    -- Resulting integer
    out_data                    : out std_logic_vector(INT_BIT_WIDTH-1 downto 0);

    -- Immediately asserted when the current input byte is the last byte of the varint
    last_byte                   : out std_logic
  );
end VarIntDecoder;

architecture behv of VarIntDecoder is
  
  -- Max size of a varint of given INT_BIT_WIDTH in bytes
  constant MAX_VARINT_SIZE            : natural := natural(CEIL(real(INT_BIT_WIDTH)/real(7)));

  -- Bit width necessary to count up to MAX_VARINT_SIZE - 1
  constant COUNT_WIDTH                : natural := log2ceil(MAX_VARINT_SIZE - 1);

  type state_t is (IDLE, DECODING);
      signal state, state_next : state_t;

  signal out_data_r                   : std_logic_vector(INT_BIT_WIDTH - 1 downto 0);
  signal out_data_r_next              : std_logic_vector(INT_BIT_WIDTH - 1  downto 0);
  signal byte_count                   : std_logic_vector(COUNT_WIDTH - 1 downto 0);
  signal byte_count_next              : std_logic_vector(COUNT_WIDTH - 1 downto 0);

begin
  
  out_data <= out_data_r;

  logic_p: process (in_data, state, byte_count)
  begin
    -- Default values
    out_data_r_next <= out_data_r;
    state_next <= state;
    byte_count_next <= byte_count;

    case state is
      when IDLE =>
        -- Keep last_byte at 0 when decoder is not decoding
        last_byte <= '0';

      when DECODING =>
        -- Count which byte we are processing
        byte_count_next <= std_logic_vector(unsigned(byte_count) + 1);

        -- If first bit in a byte is 0, no more bytes are coming
        if in_data(7) = '0' then
          last_byte <= '1';
          state_next <= IDLE;
        else
          last_byte <= '0';
        end if;

        -- Access different part of output register for each input byte
        for i in 0 to MAX_VARINT_SIZE - 1 loop
          if byte_count = std_logic_vector(to_unsigned(i, byte_count'length)) then

            -- On the last loop iteration we might not be able to fit another 7 bits into the register, so cut it short.
            if i = MAX_VARINT_SIZE - 1 then
              out_data_r_next(INT_BIT_WIDTH - 1 downto 7 * i) <= in_data(6 - (INT_BIT_WIDTH mod 7) downto 0);
            else
              out_data_r_next(7 * (i + 1) downto 7 * i) <= in_data(6 downto 0);
            end if;
          end if;
        end loop;
    end case;
  end process;


  state_p: process (clk)
  begin
    if rising_edge(clk) then
      if reset = '1' then
        state <= IDLE;
        out_data_r <= (others => '0');
        byte_count <= (others => '0');
      elsif start = '1' then
        state <= DECODING;
        out_data_r <= (others => '0');
        byte_count <= (others => '0');
      else
        state <= state_next;
        out_data_r <= out_data_r_next;
        byte_count <= byte_count_next;
      end if;
    end if;
  end process;

end architecture;