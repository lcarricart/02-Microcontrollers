------------------------------------------------------------------
--
-- [IE3-DI] Digital Circuits Winter Term 2025
--			Exercise 4
--
-- @name:   DIRISC_global.vhd
-- @author: 
-- @description: DIRISC global parameters and configuration 
--				 PACKAGE
--
-- (c) 2025 HAW Hamburg
--
------------------------------------------------------------------

---------------------------------
-- libraries 
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

---------------------------------
-- package declaration and definition 
package global is

-- opgroup constants 
constant REGREG : std_logic_vector(1 downto 0) := "00";
constant BRANCH : std_logic_vector(1 downto 0) := "01";
constant LOAD   : std_logic_vector(1 downto 0) := "10";
constant STORE  : std_logic_vector(1 downto 0) := "11";

-- opcode (ALU) constants
--constant OPCODE_MOV : std_logic_vector(3 downto 0) := ;
--constant OPCODE_ADD : std_logic_vector(3 downto 0) := ;
--constant OPCODE_SUB : std_logic_vector(3 downto 0) := ;
--constant OPCODE_MUL : std_logic_vector(3 downto 0) := ;
--constant OPCODE_AND : std_logic_vector(3 downto 0) := ;
--constant OPCODE_OR  : std_logic_vector(3 downto 0) := ;
--constant OPCODE_NOT : std_logic_vector(3 downto 0) := ;
--constant OPCODE_XOR : std_logic_vector(3 downto 0) := ;
--constant OPCODE_SHL : std_logic_vector(3 downto 0) := ;
--constant OPCODE_SHR : std_logic_vector(3 downto 0) := ;

-- branch condition constants
constant ISEQUAL  : std_logic_vector(3 downto 0) := "0000";
constant NOTEQUAL : std_logic_vector(3 downto 0) := "0001";
constant GREATER  : std_logic_vector(3 downto 0) := "0010";
constant GREATEREQUAL : std_logic_vector(3 downto 0) := "0011";
constant LESS      : std_logic_vector(3 downto 0) := "0100";
constant LESSEQUAL : std_logic_vector(3 downto 0) := "0101";
constant HIGHER    : std_logic_vector(3 downto 0) := "0110";
constant HIERSAME  : std_logic_vector(3 downto 0) := "0111";
constant LOWER     : std_logic_vector(3 downto 0) := "1000";
constant LOWERSAME : std_logic_vector(3 downto 0) := "1001";
constant ALWAYS    : std_logic_vector(3 downto 0) := "1010";

end package global;
