--[
--  RnWriter.lua
--
--  Este arquivo cria uma table contendo funções utilizadas pelo teste automático.
--
--]

RnWriter = {
    readers = 0,
    writers = 0,
    wantToWrite = 0,

    reader_blocked = function(self)
        if(self.writers == 0 and self.wantToWrite == 0) then
            print("Error: reader blocked with no writers/wantToWrite!")
        end
    end,

    reader_reading = function(self)
        if(self.writers > 0) then
            print("Error: reader reading with ".. self.writers .. " writer(s)!")
        end
        self.readers = self.readers + 1
    end,

    reader_released = function(self)
        self.readers = self.readers - 1
    end,

    writer_wantsToWrite = function(self)
        self.wantToWrite = self.wantToWrite + 1
    end,

    writer_blocked = function(self)
        if(self.writers == 0 and self.readers == 0) then
            print("Error: writer blocked with no readers/writers!")
        end
    end,

    writer_writing = function(self)
        if(self.writers > 0 or self.readers > 0) then
            print("Error: writer writing with " .. self.writers .. [[ writer(s)
                and ]] .. self.readers .. " reader(s)!")
        end
        self.wantToWrite = self.wantToWrite - 1
        self.writers = self.writers + 1
    end,

    writer_released = function(self)
        self.writers = self.writers - 1
    end,

}
