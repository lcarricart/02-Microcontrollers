------------------------------------------------------------------
--
-- [IE3-DI] Digital Circuits Winter Term 2025
--			Exercise 4
--
-- @name:   DIRISC_top.vhd
-- @author: 
-- @description: DIRISC processing unit
--				 TOPLEVEL
--
-- (c) 2025 HAW Hamburg
--
------------------------------------------------------------------

---------------------------------
-- libraries / packages 
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

---------------------------------
-- entity
entity RISC_top is
    port(
    clk : in std_logic;
    rstn : in std_logic;
    SW0  : in std_logic_vector(7 downto 0);  
    SW1  : in std_logic_vector(7 downto 0); 
    debug0 : in std_logic_vector(1 downto 0);
    LED0  : out std_logic_vector(7 downto 0);  
    LED1  : out std_logic_vector(7 downto 0)
    );  
end RISC_top;

---------------------------------
-- architecture
architecture rtl of RISC_top is

-- component declarations
component control_unit
    port(
        clk : in std_logic;
        rst : in std_logic;
        instruction : in  std_logic_vector(15 downto 0);
        SREG        : in  std_logic_vector(3 downto 0);
        addr        : out std_logic_vector(10 downto 0);
        cs          : out std_logic_vector(1 downto 0);
        wren        : out std_logic;
        opgroup     : out std_logic_vector(1 downto 0);
        opcode      : out std_logic_vector(3 downto 0);
        src0        : out std_logic_vector(2 downto 0);
        src1        : out std_logic_vector(2 downto 0);
        dst         : out std_logic_vector(2 downto 0);
        PC          : out std_logic_vector(9 downto 0)
        );
end component;

component ALU
    Port ( data_src0 : in STD_LOGIC_VECTOR (15 downto 0);
           data_src1 : in STD_LOGIC_VECTOR (15 downto 0);
           opcode : in STD_LOGIC_VECTOR(3 downto 0);
           
           data_dst  : out STD_LOGIC_VECTOR (15 downto 0)
           );
end component;

component RF
    Port ( clk : in STD_LOGIC;
           rst : in STD_LOGIC;
           
           opgroup : in STD_LOGIC_VECTOR(1 downto 0);
           src0 : in STD_LOGIC_VECTOR(2 downto 0);
           src1 : in STD_LOGIC_VECTOR(2 downto 0);
           dst : in STD_LOGIC_VECTOR(2 downto 0); 

           data_dst : in STD_LOGIC_VECTOR(15 downto 0);
          
           data_src0 : out STD_LOGIC_VECTOR (15 downto 0);
           data_src1 : out STD_LOGIC_VECTOR (15 downto 0) 
           );
    
end component;

component DMEM
  port (
    -- primary memory port (a)
    clk   : in  std_logic;						-- clock
    wr    : in  std_logic;						-- write enable flag
    cs    : in  std_logic;                      -- chipselect
    addr  : in  std_logic_vector(9 downto 0);	-- address port
    din   : in  std_logic_vector(15 downto 0);	-- data in
    dout  : out std_logic_vector(15 downto 0)	-- data out
    );
end component;

component IO
    port(
    clk   : in  std_logic;
    rst   : in  std_logic;
    SW0   : in  std_logic_vector(7 downto 0);
    SW1   : in  std_logic_vector(7 downto 0);
    cs    : in  std_logic;
    wren  : in  std_logic;
    din   : in  std_logic_vector(15 downto 0);
    addr  : in  std_logic_vector(1 downto 0);
    
    LED0  : out std_logic_vector(7 downto 0);
    LED1  : out std_logic_vector(7 downto 0);
    dout  : out std_logic_vector(15 downto 0)
    );
end component;

component PMEM
  port (
    clk   : in  std_logic;							-- clock
    PC          : in  std_logic_vector(9 downto 0);	-- PC in
    instruction : out std_logic_vector(15 downto 0)	-- instruction out
    );
end component;

-- signal declarations
signal instruction_s : std_logic_vector(15 downto 0);
signal addr_s : std_logic_vector(10 downto 0);
signal cs_s : std_logic_vector(1 downto 0);
signal opgroup_s : std_logic_vector(1 downto 0);
signal opcode_s : std_logic_vector(3 downto 0);
signal src0_s, src1_s, dst_s : std_logic_vector(2 downto 0);
signal PC_s : std_logic_vector(9 downto 0);
signal wren_s : std_logic;

signal rst : std_logic;

signal data_src0_s, data_src1_s : std_logic_vector(15 downto 0);
signal data_dst_s, data_dst_ALU_s, data_dst_DMEM_s, data_dst_IO_s : std_logic_vector(15 downto 0);
signal LED0_s, LED1_s : std_logic_vector(7 downto 0);

begin
-- output assignments
LED0 <= data_dst_s(7 downto 0) when debug0(0) = '1' else
        data_dst_s(15 downto 8) when debug0(1) = '1' else
        LED0_s;
LED1 <= LED1_s;
        
-- signal assignment/datapath
data_dst_s <= data_dst_DMEM_s when cs_s(1) = '1' else
              data_dst_IO_s when cs_s(0) = '1' else
              data_dst_ALU_s;         

rst <= not rstn;
---------------------------------
-- instantiation
i_control_unit : control_unit
    port map(
        clk => clk,
        rst => rst,
        instruction => instruction_s,
        SREG        => (others =>'0'),
        addr        => addr_s,
        cs          => cs_s,
        wren        => wren_s,
        opgroup     => opgroup_s,
        opcode      => opcode_s,
        src0        => src0_s,
        src1        => src1_s,
        dst         => dst_s,
        PC          => PC_s
        );

i_ALU : ALU
    port map( 
    data_src0 => data_src0_s,
    data_src1 => data_src1_s,
    opcode => opcode_s,
    data_dst => data_dst_ALU_s
    );

i_RF : RF
    port map (
           clk => clk,
           rst => rst,
           opgroup => opgroup_s,
           src0 => src0_s,
           src1 => src1_s,
           dst => dst_s, 
           data_dst => data_dst_s,
           data_src0 => data_src0_s,
           data_src1 => data_src1_s 
           );

i_DMEM : DMEM    
  port map(
    clk   => clk,
    wr    => wren_s,
    cs    => cs_s(1),
    addr  => addr_s(9 downto 0),
    din   => data_src0_s,
    dout  => data_dst_DMEM_s
    );
    
i_IO : IO
    port map(
    clk  => clk,
    rst  => rst,
    SW0  => SW0,
    SW1  => SW1,
    cs   => cs_s(0),
    wren => wren_s,
    din  => data_src0_s,
    addr => addr_s(1 downto 0),
    
    LED0 => LED0_s,
    LED1 => LED1_s,
    dout => data_dst_IO_s
    );    
    
i_PMEM : PMEM
  port map (
    clk => clk,
    PC  => PC_s,
    instruction => instruction_s
    );

end rtl;
