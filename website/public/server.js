const express = require('express');
const bodyParser = require('body-parser');
const mongoose = require('mongoose');
const cors = require('cors');

// Initialize Express
const app = express();

// Middleware
app.use(cors());
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

// MongoDB Connection
mongoose
  .connect('mongodb://localhost:27017/parkingSystem', { useNewUrlParser: true, useUnifiedTopology: true })
  .then(() => console.log('Connected to MongoDB'))
  .catch((err) => console.error('Failed to connect to MongoDB:', err));

// Define Mongoose Schema
const reportSchema = new mongoose.Schema({
  name: String,
  phone: String,
  carNumber: String,
  description: String,
});

const Report = mongoose.model('Report', reportSchema);

// API Endpoint to Handle Form Submission
app.post('/submit-report', async (req, res) => {

  
   
  const { name, phone, carNumber, description } = req.body;

  try {
    const newReport = new Report({ name, phone, carNumber, description });
    newReport.save();
    res.status(201).send({ alert: 'Report submitted successfully!' });
    // console.log("Report submitted successfully!");
  } catch (error) {
    console.error(error);
    res.status(500).send({ alert: 'Failed to submit report' });
  }
});

// Start Server
const PORT = 5000;
app.listen(PORT, () => console.log(`Server running on http://localhost:${PORT}`));
