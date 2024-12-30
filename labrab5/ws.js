const express = require('express');
const cors = require('cors');
const net = require('net');
const bodyParser = require('body-parser');
const Iconv = require('iconv').Iconv;

const app = express();
const PORT = 9091;
const TCP_SERVER_HOST = '127.0.0.1';
const TCP_SERVER_PORT = 9090;

app.use(cors());

app.use(bodyParser.json());

// Эндпоинт для добавления студента
app.post('/ADD_STUDENT', (req, res) => {
    const { firstName, lastName, group } = req.body;

    const tcpClient = new net.Socket();
    tcpClient.connect(TCP_SERVER_PORT, TCP_SERVER_HOST, () => {
        // Отправляем данные на TCP-сервер
        tcpClient.write("ADD_STUDENT " + firstName + "," + lastName + "," + group + ",\n");
    });

    tcpClient.on('data', (data) => {
        res.send(data.toString());
        tcpClient.destroy(); // Закрываем TCP-соединение
    });

    tcpClient.on('error', (err) => {
        console.error('Ошибка TCP-соединения:', err);
        res.status(500).send('Ошибка сервера');
    });
});

// Эндпоинт для получения студентов
app.get('/GET_STUDENTS', (req, res) => {
    const tcpClient = new net.Socket();
    tcpClient.connect(TCP_SERVER_PORT, TCP_SERVER_HOST, () => {
        tcpClient.write("GET_STUDENTS\n");
    });

    tcpClient.on('data', (data) => {
		const cdata = new Buffer(data, 'binary');
        conv = Iconv('windows-1251', 'utf8');
        data = conv.convert(cdata).toString();
		
        res.json(data);
        tcpClient.destroy(); // Закрываем TCP-соединение
    });

    tcpClient.on('error', (err) => {
        console.error('Ошибка TCP-соединения:', err);
        res.status(500).send('Ошибка сервера');
    });
});

// Эндпоинт для добавления преподавателя
app.post('/ADD_TEACHER', (req, res) => {
    const { firstName, lastName, age } = req.body;

    const tcpClient = new net.Socket();
    tcpClient.connect(TCP_SERVER_PORT, TCP_SERVER_HOST, () => {
		let data = "ADD_TEACHER " + firstName + "," + lastName + "," + age + ";\n";
		
		const cdata = new Buffer(data, 'binary');
        conv = Iconv('utf8', 'windows-1251');
        data = conv.convert(cdata);
		
        tcpClient.write(data);
    });

    tcpClient.on('data', (data) => {
        res.send(data.toString());
        tcpClient.destroy(); // Закрываем TCP-соединение
    });

    tcpClient.on('error', (err) => {
        console.error('Ошибка TCP-соединения:', err);
        res.status(500).send('Ошибка сервера');
    });
});

// Эндпоинт для получения преподавателей
app.get('/GET_TEACHERS', (req, res) => {
    const tcpClient = new net.Socket();
    tcpClient.connect(TCP_SERVER_PORT, TCP_SERVER_HOST, () => {
        tcpClient.write("GET_TEACHERS\n");
    });

    tcpClient.on('data', (data) => {
		const cdata = new Buffer(data, 'binary');
        conv = Iconv('windows-1251', 'utf8');
        data = conv.convert(cdata).toString();
		
        res.json(data);
        tcpClient.destroy(); // Закрываем TCP-соединение
    });

    tcpClient.on('error', (err) => {
        console.error('Ошибка TCP-соединения:', err);
        res.status(500).send('Ошибка сервера');
    });
});

// Запускаем сервер
app.listen(PORT, () => {
    console.log(`Сервер запущен на порту ${PORT}`);
});
